#ifndef _detect_wake_word_state_h_
#define _detect_wake_word_state_h_

class NeuralNetwork;
class AudioProcessor;

#define COMMAND_WINDOW 3

enum Command
{
    CMD_FORWARD,
    CMD_BACKWARD,
    CMD_LEFT,
    CMD_RIGHT,
    CMD_NONSENSE,
    NUMBER_COMMANDS
};

extern const char* commands[NUMBER_COMMANDS];

typedef struct
{
    int index;
    float score;
} CmdDectResult;

class CommandDetector
{
private:
    NeuralNetwork *m_nn;
    AudioProcessor *m_audio_processor;
    int16_t* input_audio;

    float m_scores[COMMAND_WINDOW][NUMBER_COMMANDS];
    int m_scores_index;
    int64_t m_last_detection;

#ifdef DEBUG
    float m_average_detect_time;
    int m_number_of_runs;
#endif

public:
    CommandDetector(int16_t* _input_audio);
    ~CommandDetector();
    CmdDectResult run();
};

#endif
