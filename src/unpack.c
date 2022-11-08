#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "serialize.h"

/* Unpack the given packet into the buffer unpacked.  You can assume
 * that packed points to a packet buffer large enough to hold the
 * packet described therein, but you cannot assume that the packet is
 * otherwise valid.  You can assume that unpacked points to a character
 * buffer large enough to store the unpacked packet, if it is a valid
 * packet.
 *
 * Returns the packet type that was unpacked, or -1 if it is invalid.
 */
int unpack(char *unpacked, void *packed)
{
    void *ptr = packed;
    // get message type
    int type = *(int *)packed;
    ptr = ptr + sizeof(int);

    // get It Name
    char name[NAME_SIZE + 1];
    strncpy(name, ptr, NAME_SIZE);
    name[NAME_SIZE] = '\0';
    ptr = ptr + sizeof(char) * NAME_SIZE;

    // get data lengths
    size_t len[3];
    int index = 0;
    while (1)
    {
        size_t size = *(size_t *)ptr;
        ptr = ptr + sizeof(size_t);
        len[index] = size;
        index++;
        if (size == 0)
        {
            break;
        }
    }

    // switch type
    switch (type)
    {
    case STATUS: // if STATUS;
    {
        // set unpacked as "ITName Message"
        char *pchar = unpacked;
        strcpy(pchar, name);
        strcat(pchar, " ");
        strcat(pchar, (char *)ptr);
        break;
    }
    case MESSAGE: // if MESSAGE
    {
        // set unpacked as "ITName: message"
        char *pchar = unpacked;
        strcpy(pchar, name);
        strcat(pchar, ": ");
        strcat(pchar, (char *)ptr);
        break;
    }
    case LABELED: // if LABELED
    {
        char *pchar = unpacked;
        // set unpacked as "ITNAME: @"
        strcpy(pchar, name);
        strcat(pchar, ": @");
        // get messge and target from data
        char message[MAX_MESSAGE_SIZE + 1];
        char target[NAME_SIZE + 1];
        // get message
        for (int i = 0; i < len[0]; i++)
        {
            *(message + i) = *(char *)ptr;
            ptr++;
        }
        message[len[0]] = '\0';
        // get target
        for (int i = 0; i < len[1]; i++)
        {
            *(target + i) = *(char *)ptr;
            ptr++;
        }
        target[len[1]] = '\0';
        // concat unpacked with "target message", finally set unpacked as "ITName: @target message"
        strcat(pchar, target);
        strcat(pchar, " ");
        strcat(pchar, message);
        break;
    }
    default:
        break;
    }
    return 0;
}

/* Unpack the given packed packet into the given statistics structure.
 * You can assume that packed points to a packet buffer large enough to
 * hold the statistics packet, but you cannot assume that it is
 * otherwise valid.  You can assume that statistics points to a
 * statistics structure.
 *
 * Returns the packet type that was unpacked, or -1 if it is invalid.
 */
int unpack_statistics(struct statistics *statistics, void *packed)
{
    // get type
    int type = *(int *)packed;
    packed += sizeof(int);
    // if type is not STATISTICS return invalid
    if (type != STATISTICS)
    {
        return -1;
    }

    // get sender
    strncpy(statistics->sender, (char *)packed, NAME_SIZE);
    statistics->sender[NAME_SIZE] = '\0';
    packed = (char *)packed + NAME_SIZE;

    // get most_active
    strncpy(statistics->most_active, (char *)packed, NAME_SIZE);
    statistics->most_active[NAME_SIZE] = '\0';
    packed = (char *)packed + NAME_SIZE;
    // get most_active_count
    statistics->most_active_count = *(int *)packed;
    packed += sizeof(int);
    // get invalid_count
    statistics->invalid_count = *(long *)packed;
    packed += sizeof(long);
    // get refresh_count
    statistics->refresh_count = *(long *)packed;
    packed += sizeof(long);
    // get message_count
    statistics->messages_count = *(int *)packed;
    packed += sizeof(int);
    return STATISTICS;
}
