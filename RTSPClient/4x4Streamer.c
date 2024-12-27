#include <SDL2/SDL.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/log.h>
#include <libavutil/opt.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>

#define NUM_STREAMS 4

typedef struct
{
    const char      *url;
    int              index;
    AVFormatContext *fmt_ctx;
    AVCodecContext  *dec_ctx;
    SDL_Texture     *texture;
    int              video_stream_index;
    pthread_mutex_t *mutex;
} StreamContext;

// Thread function to handle each stream
void *stream_handler(void *arg)
{
    StreamContext *stream = (StreamContext *)arg;
    AVPacket       pkt;
    int            ret;
    AVFrame       *frame = av_frame_alloc();
    SDL_Event      event;
    int            quit = 0;

    // Open input stream
    ret = avformat_open_input(&stream->fmt_ctx, stream->url, NULL, NULL);
    if (ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "Failed to open input stream: %s\n", av_err2str(ret));
        return NULL;
    }

    ret = avformat_find_stream_info(stream->fmt_ctx, NULL);
    if (ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "Failed to retrieve stream information: %s\n", av_err2str(ret));
        return NULL;
    }

    // Find video stream
    stream->video_stream_index = -1;
    for (int i = 0; i < stream->fmt_ctx->nb_streams; i++)
    {
        if (stream->fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            stream->video_stream_index = i;
            break;
        }
    }

    if (stream->video_stream_index == -1)
    {
        av_log(NULL, AV_LOG_ERROR, "Could not find video stream\n");
        return NULL;
    }

    // Find the decoder
    AVCodec *dec = avcodec_find_decoder(stream->fmt_ctx->streams[stream->video_stream_index]->codecpar->codec_id);
    if (!dec)
    {
        av_log(NULL, AV_LOG_ERROR, "Failed to find video decoder\n");
        return NULL;
    }

    stream->dec_ctx = avcodec_alloc_context3(dec);
    if (!stream->dec_ctx)
    {
        av_log(NULL, AV_LOG_ERROR, "Failed to allocate codec context\n");
        return NULL;
    }

    ret = avcodec_parameters_to_context(stream->dec_ctx, stream->fmt_ctx->streams[stream->video_stream_index]->codecpar);
    if (ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "Failed to copy codec parameters to codec context: %s\n", av_err2str(ret));
        return NULL;
    }

    ret = avcodec_open2(stream->dec_ctx, dec, NULL);
    if (ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "Failed to open codec: %s\n", av_err2str(ret));
        return NULL;
    }

    // Wait for the frame in the main thread to render
    while (!quit)
    {
        ret = av_read_frame(stream->fmt_ctx, &pkt);
        if (ret < 0)
        {
            break;  // End of stream or error
        }

        if (pkt.stream_index == stream->video_stream_index)
        {
            ret = avcodec_send_packet(stream->dec_ctx, &pkt);
            if (ret < 0)
            {
                av_log(NULL, AV_LOG_ERROR, "Error sending packet to decoder: %s\n", av_err2str(ret));
                break;
            }

            ret = avcodec_receive_frame(stream->dec_ctx, frame);
            if (ret == 0)
            {
                // Lock the mutex to safely update texture in the main thread
                pthread_mutex_lock(stream->mutex);
                SDL_UpdateYUVTexture(stream->texture, NULL, frame->data[0], frame->linesize[0], frame->data[1], frame->linesize[1], frame->data[2],
                                     frame->linesize[2]);
                pthread_mutex_unlock(stream->mutex);
            }
        }
        av_packet_unref(&pkt);
    }

    av_frame_free(&frame);
    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc < NUM_STREAMS + 1)
    {
        printf("Usage: %s <url1> <url2> <url3> <url4>\n", argv[0]);
        return -1;
    }

    // Initialize libavformat and register all codecs
    av_register_all();
    avformat_network_init();

    // Setup StreamContext for each stream
    StreamContext   streams[NUM_STREAMS];
    pthread_t       threads[NUM_STREAMS];
    pthread_mutex_t mutexes[NUM_STREAMS];

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
        return -1;
    }

    // Create SDL windows, textures, and mutexes in the main thread
    for (int i = 0; i < NUM_STREAMS; i++)
    {
        pthread_mutex_init(&mutexes[i], NULL);
        streams[i].url = argv[i + 1];
        streams[i].index = i;
        streams[i].mutex = &mutexes[i];

        // Create SDL window and renderer for each stream
        streams[i].texture = NULL;

        // Create SDL window and renderer
        SDL_Window *window = SDL_CreateWindow("Video Player", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
        if (!window)
        {
            fprintf(stderr, "SDL window creation failed: %s\n", SDL_GetError());
            return -1;
        }
        SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer)
        {
            fprintf(stderr, "SDL renderer creation failed: %s\n", SDL_GetError());
            return -1;
        }

        streams[i].texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_YV12, SDL_TEXTUREACCESS_STREAMING, 640, 480);
        if (!streams[i].texture)
        {
            fprintf(stderr, "SDL texture creation failed: %s\n", SDL_GetError());
            return -1;
        }

        streams[i].fmt_ctx = NULL;
        pthread_create(&threads[i], NULL, stream_handler, &streams[i]);
    }

    // Wait for all threads to finish
    for (int i = 0; i < NUM_STREAMS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // Cleanup SDL resources
    SDL_Quit();
    return 0;
}
