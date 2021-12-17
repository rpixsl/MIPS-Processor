import numpy as np


def relu(x):
    return np.maximum(0, x)


def sigmoid(x):
    return 1 / (1 + np.exp(-x))


def softmax(x):
    if x.ndim == 2:
        x = x.T
        x = x - np.max(x, axis=0)
        y = np.exp(x) / np.sum(np.exp(x), axis=0)
        return y.T

    x = x - np.max(x)
    return np.exp(x) / np.sum(np.exp(x))


class Network:
    def __init__(self):
        # basic dimension parameter
        self.L = None  # number of hidden & output layer
        self.D = None  # dimension of sample data point
        self.K = None  # number of Gaussian / classifications

        # network parameter
        self.neuron_num      = {}
        self.activation_func = {}
        self.para            = {}

        # optimizer index
        self.h = {}     # for optimizer "AdaGrad", "RMSprop"
        self.m = {}     # for optimizer "Adam"
        self.v = {}     # for optimizer "Adam"

    def initialize_network(self, dimension, neuron_num, activation_func,
                           para=None, h=None, m=None, v=None):
        # basic dimension parameter
        self.L = len(neuron_num)
        self.D = dimension
        self.K = neuron_num[self.L - 1]

        # network parameter
        self.neuron_num = neuron_num
        self.activation_func = activation_func
        if len(activation_func) != self.L:
            print('Error! Dimension of the "activation_func" not match!')

        for l in range(self.L):
            if l == 0:
                node_from = self.D
            else:
                node_from = self.neuron_num[l - 1]
            node_to   = self.neuron_num[l]

            # network parameter
            sd = 0.01
            if self.activation_func[l] == sigmoid:
                sd = np.sqrt(1 / node_from)
            elif self.activation_func[l] == relu:
                sd = np.sqrt(2 / node_from)
            self.para['w'+str(l)] = sd * np.random.randn(node_from, node_to)
            self.para['b'+str(l)] = np.zeros( (1, node_to) )

            # optimizer index
            self.h['w'+str(l)] = np.zeros( (node_from, node_to) )
            self.h['b'+str(l)] = np.zeros( (        1, node_to) )
            self.m['w'+str(l)] = np.zeros( (node_from, node_to) )
            self.m['b'+str(l)] = np.zeros( (        1, node_to) )
            self.v['w'+str(l)] = np.zeros( (node_from, node_to) )
            self.v['b'+str(l)] = np.zeros( (        1, node_to) )

        # if already have input value
        if para: self.para = para
        if h: self.h = h
        if m: self.m = m
        if v: self.v = v

    def predict(self, sample_point):
        """
        Take "sample_point" as the network's input. Using the current network
        parameters to predict the label of the "sample_point." Notes that the
        return value is not the true label like [ 0 0 1 0]. It's the score of
        each value [ 10 20 30 5 ]. To get the label, still need to take argmax
        of return value "z"

        :param sample_point:  [ sample_size * D ], np.array
        :return: [ sample_size * K ], np.array
        """
        a = sample_point
        for l in range(self.L):
            z = np.dot(a, self.para['w'+str(l)]) + self.para['b'+str(l)]
            a = self.activation_func[l](z)

        return a

    def accuracy(self, sample_point, sample_label):
        """
        Give a sample point, get the predicting label from the network. Then,
        compare the predicting label with the correct label "sample_label", and
        return the accuracy of the prediction

        correct = 0
        for n in range(sample_size):
            if y[n] == t[n]:
                correct = correct + 1
        accuracy = correct / sample_size

        :param sample_point: [ sample_size * D ], np.array
        :param sample_label: [ sample_size * K ], np.array
        :return: accuracy of the network prediction (float)
        """
        y = np.argmax(self.predict(sample_point), axis=1)
        t = np.argmax(sample_label, axis=1)

        return np.sum(y == t) / sample_point.shape[0]

    def loss(self, sample_point, sample_label):
        """
         "Ross Entropy Error"

         The t is correct label. The first layer of for loop is using to traverse
         entire sample, and the second layer of for loop is using to control
         which y value of that sample will affect the loss value. The "y" value
         will affect the loss value only when it's in the same position of
         correct label.

         Example,
                 t_i = [  0  0  1  0 ]       y_i = [ 10 20 10  2 ]
         then, for sample point i,
         loss_i = ( 0 * 10 ) + ( 0 * 20 ) + ( 1 * 10 ) + ( 0 * 2 ) = 1 * 10 = 10

         :param sample_point: [ sample_size * D ], np.array
         :param sample_label: [ sample_size * K ], np.array
         :return: loss value (float)
         """
        y = self.predict(sample_point)
        t = sample_label

        delta = 1e-7
        return -(np.sum(np.multiply(t, np.log(y + delta))) /
                 sample_point.shape[0])

    def gradient_ng(self, sample_point, sample_label):
        """
        "Numerical Gradient"

        Get the gradient of all the parameter by just forward, no backward.
        The first loop is used to go through all the parameter.
        Inside the first loop is the numerical gradient of that parameter:
        give a small change 'h', see how f, the loss function, change.

        :param sample_point: [ sample_size * D ], np.array
        :param sample_label: [ sample_size * K ], np.array
        :return: dictionary, gradient for all the parameter
        """
        grad = {}
        for key in self.para.keys():
            h = 1e-4  # 0.0001

            grad[key] = np.zeros_like(self.para[key])

            it = np.nditer(self.para[key],
                           flags=['multi_index'], op_flags=['readwrite'])
            while not it.finished:
                idx = it.multi_index
                tmp_val = self.para[key][idx]

                self.para[key][idx] = float(tmp_val) + h
                fxh1 = self.loss(sample_point, sample_label)  # f(x+h)

                self.para[key][idx] = float(tmp_val) - h
                fxh2 = self.loss(sample_point, sample_label)  # f(x-h)

                grad[key][idx] = (fxh1 - fxh2) / (2 * h)

                self.para[key][idx] = tmp_val
                it.iternext()

        return grad

    def gradient_bp(self, sample_point, sample_label):
        """
        "Backpropagation"

        Use backpropagation to find every parameters' gradient with less time
        complexity than Numerical Gradient "gradient_ng".

        We first take forward step, which same to "predict", the only different
        is we record the "a" value using a dictionary since we need to use it
        during backward step.

        For each layer, we have three step:
             a_i  ---[w_i,b_i]--->  z_i  ---[activation_func]--->  a_(i+1)
        a_i: the input of the current layer, which is also the output of
             previous layer's z_(i-1).
        z_i: a_i * w_i + b_i
        a_(i+1): activation_function (z_i)

        In backward step, we just reverse all the step above
          da_i  <---[dw_i,db_i]---  dz_i  <---[d_activation_func]---  da_(i+1)
        The only difference is that the things we got now is "d", gradient.

        :param sample_point: [ sample_size * D ], np.array
        :param sample_label: [ sample_size * K ], np.array
        :return: dictionary, gradient for all the parameter
        """
        # forward
        # a0 -> w0,b0 -> z0 -> a1 -> w1,b1 -> z1 -> a2
        a = {0: sample_point}
        for l in range(self.L):
            z = np.dot(a[l], self.para['w' + str(l)]) + self.para['b' + str(l)]
            a[l + 1] = self.activation_func[l](z)

        # backward
        # da0 <- dw0,db0 <- dz0 <- da1 <- dw1,db1 <- dz1 <- da2
        dw = {}
        db = {}

        da = 0
        for l in range(self.L-1, -1, -1):
            if l == self.L-1:
                dz = (a[l + 1] - sample_label) / len(sample_point)
            else:
                dz = da * (1.0 - a[l + 1]) * a[l + 1]
            da = np.dot(dz, self.para['w' + str(l)].T)
            dw[l] = np.dot(a[l].T, dz)
            db[l] = np.sum(dz, axis=0)

        # get gradient
        grad = {}
        for l in range(self.L):
            grad['w'+str(l)] = dw[l]
            grad['b'+str(l)] = db[l]

        return grad

    def SGD(self, grad, para):
        """
        "Stochastic Gradient Descent"

        Update all parameters including weighs and biases

        :param grad: dictionary
        :param para: dictionary, parameter that need for all optimizer
            ( will use "lr" )
        """
        for key in grad.keys():
            self.para[key] -= para["lr"] * grad[key]

    def AdaGrad(self, grad, para):
        """
        "Adaptive Gradient Algorithm", an improvement basis on "SGD" above

        Update all parameters including weighs and biases
        Can adjust learning rate by h
        At beginning, since the sum of squares of historical gradients is
        smaller, h += grads * grads the learning rate is high at first.
        As the sum of squares of historical gradients become larger, the
        learning rate will decrease

        :param grad:dictionary
        :param para: dictionary, parameter that need for all optimizer
            ( will use "lr" )
        """
        delta = 1e-7  # avoid divide zero
        for key in grad.keys():
            self.h[key] += np.square(grad[key] )
            self.para[key] -= para["lr"] * grad[key] / \
                              (np.sqrt(self.h[key]) + delta)

    def RMSprop(self, grad, para):
        """
        "Root Mean Squared Propagation", an improvement basis on "AdaGrad" above

        See "https://zhuanlan.zhihu.com/p/34230849" (Chinese)

        Update all parameters including weighs and biases
        Use "decay_rate" to control how much historical information (h) is
        retrieved.
        When the sum of squares of historical gradients is smaller,
        which means that the parameters space is gentle, (h) will give a larger
        number, which will increase the learning rate.
        When the sum of squares of historical gradients is larger, which means
        that the parameters space is steep, (h) will give a smaller number,
        which will decrease the learning rate.

        :param grad: dictionary
        :param para: dictionary, parameter that need for all optimizer
            ( will use "lr", "decay_rate" )
        """
        delta = 1e-7  # avoid divide zero
        for key in grad.keys():
            self.h[key] *= para["decay_rate"]
            self.h[key] += (1.0 - para["decay_rate"]) * np.square(grad[key])
            self.para[key] -= para["lr"] * grad[key] / \
                              (np.sqrt(self.h[key]) + delta)

    def Adam(self, grad, para):
        """
        "Adaptive Moment Estimation", an improvement basis on "RMSprop" above
        and momentum

        See "http://arxiv.org/abs/1412.6980v8" (English)

        :param grad: dictionary
        :param para: dictionary, parameter that need for all optimizer
            ( will use "lr", "beta1", "beta2", "iter" )
        """
        para["iter"] += 1
        lr_t = para["lr"] * np.sqrt(1.0 - para["beta2"]**para["iter"]) / \
               (1.0 - para["beta1"]**para["iter"])
        delta = 1e-7  # avoid divide zero
        for key in grad.keys():
            self.m[key] += (1.0 - para["beta1"]) * (grad[key] - self.m[key])
            self.v[key] += (1.0 - para["beta2"]) * (grad[key]**2 - self.v[key])
            self.para[key] -= lr_t*self.m[key] / (np.sqrt(self.v[key]) + delta)

    def train(self, sample_point, sample_label, train_number,
              gradient, optimizer, optimizer_para):
        # variable using to store train result, [ train_number ]
        sample_loss     = []
        sample_accuracy = []

        self.save_para()    # check if it can save as file correctly
        # start training, record & print training result at the same time
        for i in range(train_number):
            grad = gradient(self, sample_point, sample_label)
            optimizer(self, grad, optimizer_para)

            # store the training result
            sample_loss.append(self.loss(sample_point, sample_label))
            sample_accuracy.append(self.accuracy(sample_point, sample_label))

            # print out the training result
            print('%6d\tL: %14.11f\tA: %8.5f' %
                  (i, sample_loss[i], sample_accuracy[i]*100))
        self.save_para()

        return sample_loss, sample_accuracy

    def save_para(self):
        for key in self.para.keys():
            np.savetxt("para_{}.txt".format(key), self.para[key])
        for key in self.h.keys():
            np.savetxt("h_{}.txt".format(key), self.h[key])
        for key in self.m.keys():
            np.savetxt("m_{}.txt".format(key), self.m[key])
        for key in self.v.keys():
            np.savetxt("v_{}.txt".format(key), self.v[key])
