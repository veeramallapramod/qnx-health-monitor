
//heartbeat_client.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>

#define HEARTBEAT_CODE 1

int main(void)
{
    int coid;
    int server_pid, channel_id;

    printf("Enter Server PID  : ");
    fflush(stdout);
    if (scanf("%d", &server_pid) != 1) {
        printf("Invalid PID\n");
        return EXIT_FAILURE;
    }

    printf("Enter Channel ID  : ");
    fflush(stdout);
    if (scanf("%d", &channel_id) != 1) {
        printf("Invalid Channel ID\n");
        return EXIT_FAILURE;
    }

    printf("\nConnecting to server...\n");
    printf("PID = %d | CHID = %d\n", server_pid, channel_id);

    coid = ConnectAttach(ND_LOCAL_NODE, server_pid, channel_id, 0, 0);
    if (coid == -1) {
        perror("ConnectAttach failed");
        return EXIT_FAILURE;
    }

    printf("Connection established successfully.\n\n");

    printf("=====================================\n");
    printf("         HEARTBEAT CLIENT\n");
    printf("=====================================\n");
    printf("Client PID : %d\n", getpid());
    printf("=====================================\n\n");

    int count = 0;

    while (1)
    {
        printf("[TRANSMITTER] Sending heartbeat...\n");

        if (MsgSendPulse(coid,
                         SIGEV_PULSE_PRIO_INHERIT,
                         HEARTBEAT_CODE,
                         getpid()) == -1)
        {
            perror("MsgSendPulse failed");
            break;
        }

        sleep(2);
        count++;

        if (count == 10)
        {
            printf("\n[CLIENT] Simulating failure...\n");
            exit(0);
        }
    }

    return EXIT_SUCCESS;
}
