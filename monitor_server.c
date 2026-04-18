//monitor_server.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <signal.h>

#define HEARTBEAT_CODE  1
#define TIMER_CODE      2
#define TIMEOUT_LIMIT   3   


typedef struct {
    struct _pulse pulse;
} pulse_msg_t;

int main(void)
{
    int chid, rcvid;
    pulse_msg_t msg;

    int tick_count = 0;        
    int last_heartbeat = 0;    
    pid_t client_pid = 0;      

    
    chid = ChannelCreate(0);
    if (chid == -1) {
        perror("ChannelCreate failed");
        return EXIT_FAILURE;
    }

    printf("\n=====================================\n");
    printf("       HEALTH MONITOR SERVER\n");
    printf("=====================================\n");
    printf("Server PID  : %d\n", getpid());
    printf("Channel ID  : %d\n", chid);
    printf("Status      : Waiting for client...\n");
    printf("=====================================\n\n");


    int coid = ConnectAttach(ND_LOCAL_NODE, getpid(), chid, 0, 0);
    if (coid == -1) {
        perror("ConnectAttach failed");
        return EXIT_FAILURE;
    }

    timer_t timerid;
    struct sigevent event;
    struct itimerspec itime;

    event.sigev_notify = SIGEV_PULSE;
    event.sigev_coid = coid;
    event.sigev_priority = SIGEV_PULSE_PRIO_INHERIT;
    event.sigev_code = TIMER_CODE;

    if (timer_create(CLOCK_MONOTONIC, &event, &timerid) == -1) {
        perror("timer_create failed");
        return EXIT_FAILURE;
    }

    itime.it_value.tv_sec = 2;
    itime.it_value.tv_nsec = 0;
    itime.it_interval.tv_sec = 2;
    itime.it_interval.tv_nsec = 0;

    timer_settime(timerid, 0, &itime, NULL);

    while (1)
    {
        rcvid = MsgReceive(chid, &msg, sizeof(msg), NULL);

        if (rcvid == -1) {
            perror("MsgReceive failed");
            continue;
        }

        if (rcvid == 0)
        {
            if (msg.pulse.code == HEARTBEAT_CODE)
            {
                client_pid = msg.pulse.value.sival_int;
                last_heartbeat = tick_count;

                printf("[RECEIVER] Heartbeat received from PID %d\n", client_pid);
            }

            else if (msg.pulse.code == TIMER_CODE)
            {
                tick_count++;

                if (client_pid != 0 &&
                    (tick_count - last_heartbeat) > TIMEOUT_LIMIT)
                {
                    printf("\n[ALERT] Process %d is NOT responding (Timeout)\n\n", client_pid);
                    client_pid = 0;
                }
            }
        }
    }

    return EXIT_SUCCESS;
}