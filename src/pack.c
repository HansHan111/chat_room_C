#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pwd.h>
#include <unistd.h>
#include "serialize.h"

// get IT Name
char *getITName()
{
    struct passwd *pwd = getpwuid(getuid());
    return pwd->pw_name;
}

// pack common messages(STATUS, LABELED, MESSAGE)
void messagePack(void *pack, int type, char *name, size_t len[], char *data)
{
    void *ptr = pack;
    // set Type
    *(int *)ptr = type;
    ptr = ptr + sizeof(int);

    // set IT Name, set remain point as '\0'
    int flag = 1;
    for (int i = 0; i < NAME_SIZE; i++)
    {
        if (flag)
            *(char *)ptr = *(name + i);
        else
            *(char *)ptr = '\0';
        if (*(name + i) == '\0')
            flag = 0;
        ptr = ptr + sizeof(char);
    }

    // set Data Lengths
    while ((*(size_t *)ptr = *len) != 0)
    {
        ptr = ptr + sizeof(size_t);
        len++;
    }
    // add size_t(0)
    *(size_t *)ptr = (size_t)0;
    ptr = ptr + sizeof(size_t);
    // set data
    while (*data != '\0')
    {
        *(char *)ptr = *data;
        ptr++;
        data++;
    }
    *(char *)ptr = '\0';
}

/* Pack the user input provided in input into the appropriate message
 * type in the space provided by packed.  You can assume that input is a
 * NUL-terminated string, and that packed is a buffer of size
 * PACKET_SIZE.
 *
 * Returns the packet type for valid input, or -1 for invalid input.
 */
int pack(void *packed, char *input)
{

    int type = -1;
    char *ptr = input;
    // if input starts with '/me'
    if (*ptr == '/' && *(ptr + 1) == 'm' && *(ptr + 2) == 'e')
    {
        // if followed space after'/me'
        if (*(ptr + 3) == ' ')
        {
            // set type as STATUS
            type = STATUS;
            ptr = ptr + 3;
            // skip spaces;
            while (*ptr == ' ')
            {
                ptr++;
            }
            // calculate data_lengthes
            size_t len[2] = {
                strlen(ptr),
                0};
            // pack common message
            messagePack(packed, type, getITName(), len, ptr);
        }
        // else return invalid
        else
        {
            return -1;
        }
    }
    // if message start with '@'
    else if (*ptr == '@')
    {
        // set type as LABELED;
        type = LABELED;
        ptr++;
        char target[NAME_SIZE + 1];
        char *pt = target;
        // get target name
        while (*ptr != ' ')
        {
            *pt = *ptr;
            pt++;
            ptr++;
        }
        *pt = '\0';

        // remove spaces
        while (*ptr == ' ')
        {
            ptr++;
        }

        // calculate datalenths,
        size_t len[3] = {
            (size_t)strlen(ptr),
            (size_t)strlen(target),
            0};

        // concat message and target
        int totalLen = strlen(target) + strlen(ptr);
        char data[totalLen + 1];
        strcpy(data, ptr);
        strcpy(data + strlen(ptr), target);
        data[totalLen + 1] = '\0';
        // pack common messages
        messagePack(packed, type, getITName(), len, data);
    }
    // if message start with '/stats'
    else if (*ptr == '/' && *(ptr + 1) == 's' && *(ptr + 2) == 't' && *(ptr + 3) == 'a' && *(ptr + 4) == 't' && *(ptr + 5) == 's')
    {
        //'if followed by the end of input
        if (*(ptr + 6) == '\0')
        {
            type = STATISTICS;
            // set type;

            void *pp = packed;
            // set type on packed;
            *(int *)pp = type;
            pp = (int *)pp + 1;
            // set IT Name on packed
            char name[NAME_SIZE + 1];
            strcpy(name, getITName());
            int flag = 1;
            for (int i = 0; i < NAME_SIZE; i++)
            {
                if (flag)
                    *(char *)pp = *(name + i);
                else
                    *(char *)pp = '\0';
                if (*(name + i) == '\0')
                    flag = 0;
                pp = pp + sizeof(char);
            }
            // add 0 on end of packed
            *(size_t *)pp = (size_t)0;
        }
        // else return invalid
        else
        {
            return -1;
        }
    }
    // otherwise the type is MESSAGE
    else
    {
        // set type as MESSAGE
        type = MESSAGE;
        // calculate data length
        size_t len[2] = {
            strlen(input),
            0};
        // pack common messages
        messagePack(packed, type, getITName(), len, input);
    }
    return type;
}

/* Create a refresh packet for the given message ID.  You can assume
 * that packed is a buffer of size PACKET_SIZE.
 *
 * You should start by implementing this method!
 *
 * Returns the packet type.
 */
int pack_refresh(void *packed, int message_id)
{
    void *ptr = packed;
    // set type
    *(int *)ptr = REFRESH;
    ptr = (int *)ptr + 1;
    // set IT name
    char *name = getITName();
    int flag = 1;
    for (int i = 0; i < NAME_SIZE; i++)
    {
        if (flag)
            *(char *)ptr = *(name + i);
        else
            *(char *)ptr = '\0';
        if (*(name + i) == '\0')
            flag = 0;
        ptr = (char *)ptr + 1;
    }
    // set message_id
    *(int *)ptr = message_id;
    return REFRESH;
}
