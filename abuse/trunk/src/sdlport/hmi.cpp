// Load Abuse HMI files and covert them to standard Midi format
//
// HMI files differ from Midi files in the following ways:
// - there is a header giving offsets to the tracks and various other information (unknown)
// - note-on events include the duration of the note instead of dedicated note-off events
// - additional 0xFE event with variable length, purpose unknown
//
// This converter does the bare minimum to get Abuse HMI files to convert. The bpm and
// header information is fixed and not read from the file (except the number of tracks).
// HMI files make use of running status notation, the converted files don't.

#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#define MAX_NOTE_OFF_EVENTS 30

typedef struct
{
    unsigned int time;
    unsigned char command;
    unsigned char note;
}
midi_note_off_event_t;

midi_note_off_event_t note_off_events[MAX_NOTE_OFF_EVENTS];



unsigned int get_int_from_buffer(unsigned char* buffer)
{
    return (buffer[3] << 24) + (buffer[2] << 16) + (buffer[1] << 8) + (buffer[0]);
}


void write_big_endian_number(unsigned int le, unsigned char* buffer)
{
    buffer[3] = (le & 0x000000FF);
    buffer[2] = (le & 0x0000FF00) >> 8;
    buffer[1] = (le & 0x00FF0000) >> 16;
    buffer[0] = (le & 0xFF000000) >> 24;
}


int compare_times(const void* a, const void* b)
{
    if (((midi_note_off_event_t*)a)->time < ((midi_note_off_event_t*)b)->time)
        return -1;
    else if (((midi_note_off_event_t*)a)->time > ((midi_note_off_event_t*)b)->time)
        return 1;
    else
        return 0;
}


// Variable length number code from: http://www.chriswareham.demon.co.uk/midifiles/variable_length.html

unsigned int read_time_value(unsigned char* &buffer)
{
    unsigned int value;
    unsigned char c;

    if ((value = *buffer++) & 0x80)
    {
        value &= 0x7F;
        do
        {
            value = (value << 7) + ((c = *buffer++) & 0x7F);
        }
        while (c & 0x80);
    }

    return value;
}


void write_time_value(unsigned int time, unsigned char* &buffer)
{
    unsigned int value_buffer = time & 0x7F;

    while (time >>= 7)
    {
        value_buffer <<= 8;
        value_buffer |= ((time & 0x7F) | 0x80);
    }

    while (1)
    {
        *buffer++ = value_buffer;
        if (value_buffer & 0x80)
            value_buffer >>= 8;
        else
            break;
    }
}


void remember_note_off_event(unsigned int time, unsigned char command, unsigned char note)
{
    int i;
    for (i = 0; i < MAX_NOTE_OFF_EVENTS; i++)
    {
        if (note_off_events[i].time == 0xFFFFFFFF)
        {
            note_off_events[i].time = time;
            note_off_events[i].command = command;
            note_off_events[i].note = note;
            break;
        }
    }

    // Sort the note off array by the time
    qsort(note_off_events, MAX_NOTE_OFF_EVENTS, sizeof(midi_note_off_event_t), compare_times);
}


void check_for_note_off_events(unsigned int &current_time, unsigned int &last_time, unsigned char* &buffer)
{
    int i;
    for (i = 0; i < MAX_NOTE_OFF_EVENTS; i++)
    {
        if (note_off_events[i].time == 0xFFFFFFFF)
            break;

        if (note_off_events[i].time < current_time)
        {
            // Add event
            write_time_value(note_off_events[i].time - last_time, buffer);
            last_time = note_off_events[i].time;

            *buffer++ = note_off_events[i].command;
            *buffer++ = note_off_events[i].note;
            *buffer++ = 0x00;

            // Remove event from queue
            note_off_events[i].time = 0xFFFFFFFF;
        }
    }

    // Sort the note off array by the time
    qsort(note_off_events, MAX_NOTE_OFF_EVENTS, sizeof(midi_note_off_event_t), compare_times);
}


void convert_hmi_track(unsigned char* input, unsigned int input_size, unsigned char* &output)
{
    int done = 0;
    unsigned char current_command = 0;
    unsigned char current_value = 0;
    unsigned int current_time = 0;
    unsigned int last_time = 0;
    unsigned char* start_of_buffer = output;
    unsigned char* start_of_input = input;

    memset(note_off_events, 0xFF, sizeof(midi_note_off_event_t) * MAX_NOTE_OFF_EVENTS);

    // Midi data offset is at 0x57 from track start
    input += input[0x57];

    // Write track header, leave length as zero for now
    unsigned char track_header[] = { 0x4D, 0x54, 0x72, 0x6B, 0x00, 0x00, 0x00, 0x00, 0x00};
    memcpy(output, track_header, 8);
    output += 8;

    while (!done)
    {
        // Read variable length time
        current_time += read_time_value(input);

        // Next comes either a command (>= 0x80) or data (running status)
        current_value = *input++;
        if (current_value >= 0x80)
        {
            // Is command, make current, increase data pointer
            current_command = current_value;
            current_value = *input++;
        }

        // Check if note off events have to be inserted here
        check_for_note_off_events(current_time, last_time, output);

        if (current_command != 0xFE)
        {
            // Write variable length time to output
            write_time_value(current_time - last_time, output);
            last_time = current_time;
        }

        // Write command, no running status in output
        if (current_command != 0xFE)
            *output++ = current_command;

        switch (current_command & 0xF0)
        {
            // 1 data byte
        case 0xC0: // Program change
        case 0xD0: // Channel aftertouch
            *output++ = current_value;
            break;

            // 2 data bytes
        case 0x80: // Note off, does not occur in HMI
        case 0xA0: // Aftertouch
        case 0xB0: // Controller
        case 0xE0: // Pitch bend
            *output++ = current_value;
            *output++ = *input++;
            break;

            // 3 data bytes
        case 0x90: // Note on, non-standard, HMI files specify the duration as a third param
            *output++ = current_value;
            *output++ = *input++;
            remember_note_off_event(current_time + read_time_value(input), current_command, current_value);
            break;

        case 0xF0: // Meta event
            if (current_command == 0xFE)
            {
                // HMI specific event, variable length depending on type
                switch (current_value)
                {
                case 0x10:
                    input += 2;
                    input += *input;
                    input += 5;
                    break;
                case 0x14:
                    input += 2;
                    break;
                case 0x15:
                    input += 6;
                    break;
                }
            }
            else
            {
                // Only process end marker
                *output++ = current_value;
                *output++ = *input++;
                done = 1;
            }
            break;

        default:
            // error?
            break;
        }

        if ((unsigned int)(input - start_of_input) >= input_size)
            break;
    }

    // Write end marker if necessary
    if (done != 1)
    {
        unsigned char end_marker[] = { 0x00, 0xFF, 0x2F, 0x00 };
        memcpy(output, end_marker, 4);
        output += 4;
    }

    // Update header with length of track
    write_big_endian_number((unsigned int)(output - start_of_buffer - 8), &start_of_buffer[4]);
}


unsigned char* load_hmi(char* filename, unsigned int &data_size)
{
    unsigned char* input_buffer;
    unsigned char* output_buffer;

    FILE* hmifile = fopen(filename, "rb");

    if (hmifile == NULL)
        return NULL;

    fseek(hmifile, 0, SEEK_END);
    int buffersize = ftell(hmifile);
    fseek(hmifile, 0, SEEK_SET);

    input_buffer = (unsigned char*)malloc(buffersize);
    fread(input_buffer, 1, buffersize, hmifile);
    fclose(hmifile);

    output_buffer = (unsigned char*)malloc(buffersize * 10); // Midi files can be larger than HMI files
    unsigned char* output_buffer_ptr = output_buffer;

    // Offset to tracks is at 0x113
    unsigned int offset_tracks = get_int_from_buffer(&input_buffer[0xE8]);
    unsigned int next_offset = get_int_from_buffer(&input_buffer[0xF4]);

    unsigned char num_tracks = (next_offset - offset_tracks) / sizeof(int);

    // Write Midi file header
    unsigned char midi_header[] = { 0x4D, 0x54, 0x68, 0x64, 0x00, 0x00, 0x00, 0x06, 0x00, 0x01, 0x00, (num_tracks + 1), 0x00, 0xC0 };
    memcpy(output_buffer_ptr, midi_header, 14);
    output_buffer_ptr += 14;

    // Write additional first track with bpm info
    unsigned char bpm_track[] = { 0x4D, 0x54, 0x72, 0x6B, 0x00, 0x00, 0x00, 0x0B, 0x00, 0xFF, 0x51, 0x03, 0x18, 0x7F, 0xFF, 0x00, 0xFF, 0x2F, 0x00 };
    memcpy(output_buffer_ptr, bpm_track, sizeof(bpm_track));
    output_buffer_ptr += sizeof(bpm_track);

    int i;
    for (i = 0; i < num_tracks; i++)
    {
        unsigned int trackposition = get_int_from_buffer(&input_buffer[offset_tracks + i * (sizeof(int))]);
        unsigned int tracksize;
        if (i == num_tracks - 1)
            tracksize = buffersize - trackposition;
        else
            tracksize = get_int_from_buffer(&input_buffer[offset_tracks + (i + 1) * (sizeof(int))]) - trackposition;

        convert_hmi_track(&input_buffer[trackposition], tracksize, output_buffer_ptr);
    }

    data_size = (unsigned int)(output_buffer_ptr - output_buffer);
    output_buffer = (unsigned char*)realloc(output_buffer, data_size);

    free(input_buffer);

    return output_buffer;
}

