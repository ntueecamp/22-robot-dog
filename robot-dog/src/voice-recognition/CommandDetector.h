#ifndef _detect_wake_word_state_h_
#define _detect_wake_word_state_h_

class NeuralNetwork;
class AudioProcessor;

#define NUMBER_COMMANDS 4
#define COMMAND_WINDOW 3

extern const char* commands[NUMBER_COMMANDS];

typedef struct
{
    float score;
    int index;
} CmdDectResult;

class CommandDetector
{
private:
    NeuralNetwork *m_nn;
    AudioProcessor *m_audio_processor;
    int16_t* input_audio;

    float m_scores[COMMAND_WINDOW][NUMBER_COMMANDS];
    int m_scores_index;

    unsigned long m_last_detection;
    float m_average_detect_time;
    int m_number_of_runs;

public:
    CommandDetector(int16_t* _input_audio);
    ~CommandDetector();
    CmdDectResult run();
};

#endif
