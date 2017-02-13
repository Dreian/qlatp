// neural_net.cpp
// Implementation of a completely connected feedforward neural network with one
// hidden layer

#include <array>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <set>
#include <utility>
#include <vector>
#include "neural_net.h"

#ifndef DEBUG
#define DEBUG 0
#endif

// A helper function for generating a random double value
double gen_rand(double low, double high)
{
    double r = static_cast<double> (rand()) / static_cast<double> (RAND_MAX);
    return low + r * (high - low);
}

double sigmoid(double x)
{
    return 1.0 / (1.0 + exp(-x));
}

// Constructor for a neural network, taking layer sizes, learning rate and
// the number of steps in gradient descent
neural_net::neural_net(int input_size, int hidden_size, int output_size,
                       double learn_coeff, int desc_steps) :
    input_neurons_length(input_size),
    hidden_neurons_length(hidden_size),
    output_neurons_length(output_size),
    learn_rate(learn_coeff),
    descent_steps(desc_steps),
    hidden_neuron_weights(hidden_size, std::vector<double>(input_size, 0.0)),
    output_neuron_weights(output_size, std::vector<double>(hidden_size, 0.0)),
    hidden_neuron_bias(hidden_size, 0.0),
    output_neuron_bias(output_size, 0.0),
    hidden_neuron_weights_der(hidden_size, std::vector<double>(input_size, 0.0)),
    output_neuron_weights_der(output_size, std::vector<double>(hidden_size, 0.0)),
    hidden_neuron_bias_der(hidden_size, 0.0),
    output_neuron_bias_der(output_size, 0.0),
    current_inputs(input_size, 0.0),
    hidden_values(hidden_size, 0.0),
    current_outputs(output_size, 0.0),
    current_output_est(output_size, 0.0)
{
    for (int i = 0; i < hidden_size; i++) {
        hidden_neuron_bias[i] = gen_rand(0.0, 1.0);
        for (int j = 0; j < input_size; j++) {
            hidden_neuron_weights[i][j] = gen_rand(0.0, 1.0);
        }
    }
    for (int i = 0; i < output_size; i++) {
        output_neuron_bias[i] = gen_rand(0.0, 1.0);
        for (int j = 0; j < hidden_size; j++) {
            output_neuron_weights[i][j] = gen_rand(0.0, 1.0);
        }
    }
}

// A method for computing the outputs of the neural network for a given method,
// with the side effect of setting neuron outputs
std::vector<double>& neural_net::feed_forward(std::vector<double>& inputs)
{
    if (inputs.size() == input_neurons_length) {
        for (int i = 0; i < hidden_neurons_length; i++) {
            hidden_values[i] = hidden_neuron_bias[i];
            for (int j = 0; j < input_neurons_length; j++) {
                hidden_values[i] += hidden_neuron_weights[i][j] * inputs[j];
            }
            hidden_values[i] = sigmoid(hidden_values[i]);
//            std::cout << "   " << hidden_values[i] << std::endl;
        }
        for (int i = 0; i < output_neurons_length; i++) {
            current_output_est[i] = output_neuron_bias[i];
            for (int j = 0; j < hidden_neurons_length; j++) {
                current_output_est[i] += output_neuron_weights[i][j]
                                         * hidden_values[j];
            }
        }
        return current_output_est;
    } else {
        throw 1;
    }
}

// Compute the derivative of the error with respect to the bias of a particular
// output neuron
double neural_net::der_output_bias(int out_idx)
{
    return current_output_est[out_idx] - current_outputs[out_idx];
}

// Compute the derivative of the error with respect to a particular weight for
// one of the output neuron inputs
double neural_net::der_output_weight(int out_idx, int mid_idx)
{
    return (current_output_est[out_idx] - current_outputs[out_idx])
         * hidden_values[mid_idx];
}

// Compute the derivative of the error with respect to the bias of a particular
// hidden neuron
double neural_net::der_hidden_bias(int mid_idx)
{
    double err_sum = 0.0;
    for (int i = 0; i < output_neurons_length; i++) {
        err_sum += (current_output_est[i] - current_outputs[i])
                   * output_neuron_weights[i][mid_idx]
                   * hidden_values[mid_idx] * (1.0 - hidden_values[mid_idx]);
    }
    return err_sum;
}

// Compute the derivative of the error with respect to a particular weight for
// one of the hidden neuron inputs
double neural_net::der_hidden_weight(int mid_idx, int in_idx)
{
    double err_sum = 0.0;
    for (int i = 0; i < output_neurons_length; i++) {
        err_sum += (current_output_est[i] - current_outputs[i])
                   * output_neuron_weights[i][mid_idx]
                   * hidden_values[mid_idx] * (1.0 - hidden_values[mid_idx])
                   * current_inputs[in_idx];
    }
    return err_sum;
}

// Backpropagation algorithm, consider (input, output) pair and perform the
// gradient descent iteration a given number of times
void neural_net::back_propagate(std::vector<std::vector<double> >& in_batch,
                                std::vector<std::vector<double> >& out_batch)
{
    if (in_batch.size() != out_batch.size()) {
        throw 1;
    }
    int samples = in_batch.size();
    double curr_der = 0.0;
    for (int iter = 0; iter < descent_steps; iter++) {
        for (int i = 0; i < hidden_neurons_length; i++) {
            hidden_neuron_bias_der[i] = 0.0;
            for (int j = 0; j < input_neurons_length; j++) {
                hidden_neuron_weights_der[i][j] = 0.0;
            }
        }
        for (int i = 0; i < output_neurons_length; i++) {
            output_neuron_bias_der[i] = 0.0;
            for (int j = 0; j < hidden_neurons_length; j++) {
                output_neuron_weights_der[i][j] = 0.0;
            }
        }
        for (int sample = 0; sample < samples; sample++) {
            current_inputs = in_batch[sample];
            current_outputs = out_batch[sample];
            feed_forward(current_inputs);
            for (int i = 0; i < hidden_neurons_length; i++) {
                curr_der = der_hidden_bias(i);
                hidden_neuron_bias_der[i] += curr_der;
                for (int j = 0; j < input_neurons_length; j++) {
                    curr_der = der_hidden_weight(i, j);
                    hidden_neuron_weights_der[i][j] += curr_der;
                }
            }
            for (int i = 0; i < output_neurons_length; i++) {
                curr_der = der_output_bias(i);
                output_neuron_bias_der[i] += curr_der;
                for (int j = 0; j < hidden_neurons_length; j++) {
                    curr_der = der_output_weight(i, j);
                    output_neuron_weights_der[i][j] += curr_der;
                }
            }
        }
        for (int i = 0; i < hidden_neurons_length; i++) {
            hidden_neuron_bias[i] -= learn_rate * hidden_neuron_bias_der[i];
            for (int j = 0; j < input_neurons_length; j++) {
                hidden_neuron_weights[i][j] -= learn_rate * hidden_neuron_weights_der[i][j];
            }
        }
        for (int i = 0; i < output_neurons_length; i++) {
            output_neuron_bias[i] -= learn_rate * output_neuron_bias_der[i];
            for (int j = 0; j < hidden_neurons_length; j++) {
                output_neuron_weights[i][j] -= learn_rate * output_neuron_weights_der[i][j];
            }
        }
    }
}

void neural_net::print(void)
{
    if (DEBUG) {
        std::cout << "HIDDEN" << std::endl;
        for (int i = 0; i < hidden_neurons_length; i++) {
            std::cout << i << " free: " << hidden_neuron_bias[i] << std::endl;
            for (int j = 0; j < input_neurons_length; j++) {
                std::cout << j << "->" << i << ": " << hidden_neuron_weights[i][j] << std::endl;
            }
        }
        std::cout << "OUTPUT" << std::endl;
        for (int i = 0; i < output_neurons_length; i++) {
            std::cout << i << " free: " << output_neuron_bias[i] << std::endl;
            for (int j = 0; j < hidden_neurons_length; j++) {
                std::cout << j << "->" << i << ": " << output_neuron_weights[i][j] << std::endl;
            }
        }
    }
}

int main(void)
{
    srand(time(0));
    neural_net nn(2, 5, 1, 0.001, 100000);
    std::vector<std::vector<double> > x(40, std::vector<double>(2, 0.0));
    std::vector<std::vector<double> > y(40, std::vector<double>(1, 0.0));
    for (int i = 0; i < 40; i++) {
        x[i][0] = gen_rand(-3.5, 3.5);
        x[i][1] = gen_rand(-3.5, 3.5);
        y[i][0] = x[i][0] * x[i][1];
        /*x[i][2] = x[i][0] * x[i][1];*/
        std::cout << "(" << x[i][0] << "," << x[i][1] << "): " << y[i][0] << std::endl;
    }
    nn.back_propagate(x, y);
    double err = 0.0;
    for (int i = 0; i < 40; i++) {
        err += ((nn.feed_forward(x[i]))[0] - y[i][0]) * ((nn.feed_forward(x[i]))[0] - y[i][0]);   
        std::cout << ((nn.feed_forward(x[i]))[0] - y[i][0]) << std::endl;
    }
    nn.print();
    std::cout << "TRAINING SET ERROR: " << err << std::endl;
    std::vector<double> xs(2);
    for (int j = -4; j <= 4; j++) {
        for (int i = -4; i <= 4; i++) {
            xs[0] = i; xs[1] = j; //xs[2] = xs[0] * xs[1];
            std::cout << "Estimate at (" << i << ", " << j << "): ";
            std::cout << nn.feed_forward(xs)[0] << std::endl;
        }
    }
    return 0;
}

