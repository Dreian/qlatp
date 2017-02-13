// neural_net.h
// Header file for a feedforward neural network with one hidden layer

#include <vector>

// completely connected feedforward neural network with one hidden layer
class neural_net
{
    private:
        // number of neurons in every layer
        int input_neurons_length;
        int hidden_neurons_length;
        int output_neurons_length;
        // learning rate of neural net
        double learn_rate;
        // number of steps in gradient descent
        int descent_steps;
        // neural network weights
        std::vector<std::vector<double> > hidden_neuron_weights;
        std::vector<std::vector<double> > output_neuron_weights;
        std::vector<double> hidden_neuron_bias;
        std::vector<double> output_neuron_bias;
        // neural network weight gradients, intermediate step
        std::vector<std::vector<double> > hidden_neuron_weights_der;
        std::vector<std::vector<double> > output_neuron_weights_der;
        std::vector<double> hidden_neuron_bias_der;
        std::vector<double> output_neuron_bias_der;
        // neural network internal values
        std::vector<double> current_inputs;
        std::vector<double> current_outputs;
        std::vector<double> current_output_est;
        std::vector<double> hidden_values;
    public:
        // constructor, takes sizes of layers and training parameters
        neural_net(int, int, int, double, int);
        // compute outputs of the neural network
        std::vector<double>& feed_forward(std::vector<double>&);
        // derivatives of squared errors
        double der_output_bias(int);
        double der_output_weight(int, int);
        double der_hidden_bias(int);
        double der_hidden_weight(int, int);
        // backpropagation, consider (input, output) pair
        void back_propagate(std::vector<std::vector<double> >&,
                            std::vector<std::vector<double> >&);
        void print(void);
};

