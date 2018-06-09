import serial, binascii
import matplotlib.pyplot as plt
import matplotlib.animation as animation

#Suponiendo un rango de 0 a 50
def toCelsius(value):
    return float((value*50)/5)

def getColor(value):
    if value < 10:
        return 'b'
    elif value > 28:
        return 'r'
    else:
        return 'g'

def main():
    ser = serial.Serial('/dev/ttyUSB0', 9600)
    parteB = ""
    y = [0]*50

    fig = plt.figure()
    ax = fig.add_subplot(111)
    ln, = ax.plot(y)
    ax.set_xlim([0, 51])
    ax.set_ylim([0, 51])
    plt.ion()
    plt.show()

    while True:
        data = ser.read()
        if data:
            binary = list(bin(int(data.hex(), 16))[2:].zfill(8))
            if binary[0] == "0":
                parteB = binary
            else:
                full_bin = int(''.join(binary[3:] + parteB[3:]), 2)
                converted = toCelsius(float((full_bin*5)/1023))
                y.pop(0)
                y.append(converted)
                t = "Temperatura: " + str("{0:.2f}".format(converted)) + "°C"
                plt.title(t)
                ln.set_color(getColor(converted))
                ln.set_ydata(y)
                plt.pause(0.005)

if __name__ == '__main__':
    main()