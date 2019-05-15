import serial
import matplotlib.pyplot as plt

def main():
    ser = serial.Serial('COM8',9600)
    ser.write(0x72)

    raw = []
    maf = []
    irr = []
    fir = []

    for i in range(99):

        line = ser.readline()
        print(line)
        data = line.split(', ')

        raw.append(data[1])
        maf.append(data[2])
        irr.append(data[3])
        fir.append(data[4])

    raw = list(map(float,raw))
    print(raw)

    maf = list(map(float,maf))
    print(maf)

    irr = list(map(float,irr))
    print(irr)

    fir = list(map(float,fir))
    print(fir)

    plt.plot(raw, label='raw')
    plt.plot(maf, label='MAF')
    plt.plot(irr, label='IIR')
    plt.plot(fir, label='FIR')
    plt.legend()
    plt.show()



if __name__ == '__main__': main()


