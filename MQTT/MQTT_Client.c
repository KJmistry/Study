#include <mosquitto.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MQTT_HOST  "localhost"
#define MQTT_PORT  1883
#define MQTT_TOPIC "test/topic"

/*
 *  Install mosquitto : sudo apt install mosquitto mosquitto-clients libmosquitto-dev
 */

void on_connect(struct mosquitto *mosq, void *obj, int rc)
{
    if (rc)
    {
        printf("Error: Connection failed\n");
        return;
    }
    printf("Connected to MQTT broker.\n");

    // Subscribe to topic
    mosquitto_subscribe(mosq, NULL, MQTT_TOPIC, 0);
}

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
{
    printf("Received message: %s\n", (char *)msg->payload);
}

int main(int argc, char *argv[])
{
    mosquitto_lib_init();
    struct mosquitto *mosq = mosquitto_new(NULL, true, NULL);

    if (!mosq)
    {
        fprintf(stderr, "Failed to create Mosquitto instance.\n");
        return 1;
    }

    mosquitto_connect_callback_set(mosq, on_connect);
    mosquitto_message_callback_set(mosq, on_message);

    if (mosquitto_connect(mosq, MQTT_HOST, MQTT_PORT, 60))
    {
        fprintf(stderr, "Unable to connect to MQTT broker.\n");
        return 1;
    }

    // Run in a loop
    mosquitto_loop_start(mosq);

    if (argc > 1)
    {
        printf("Publishing message: %s\n", argv[1]);
        mosquitto_publish(mosq, NULL, MQTT_TOPIC, strlen(argv[1]), argv[1], 0, false);
    }

    printf("Press Enter to exit...\n");
    getchar();

    mosquitto_loop_stop(mosq, true);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    return 0;
}
