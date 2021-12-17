import numpy as np
from matplotlib import pyplot as plt

if __name__ == "__main__":

    fig, axs = plt.subplots(1)
    axs.plot( np.loadtxt("Train_Loss.txt"), label="01" )
    plt.show()

