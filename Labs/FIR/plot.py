import matplotlib.pyplot as plt
import numpy as np
from scipy.signal import kaiserord, lfilter, firwin, freqz

b = np.array([53, 0, -91, 0, 313, 500, 313, 0, -91, 0, 53])
a = [1]

w, h = freqz(b,a)

plt.figure(1)
plt.plot((w/np.pi), 20*np.log10(np.abs(h)), linewidth=2)
plt.xlabel('Normalized frequency (0~fs/2->0~1)')
plt.ylabel('Gain (dB)')
plt.title('Original Frequency Response')
plt.xlim(0, 1)
plt.grid(True)


plt.show()

b1 = b / np.sum(b)
b2 = b / 1024
_, h1 = freqz(b1,a)
_, h2 = freqz(b2,a)

plt.figure(2)
plt.plot((w/np.pi), 20*np.log10(np.abs(h1)), linewidth=2)
plt.plot((w/np.pi), 20*np.log10(np.abs(h2)), '--',linewidth=2)
plt.xlabel('Normalized frequency (0~fs/2->0~1)')
plt.ylabel('Gain (dB)')
plt.title('Optimized Frequency Response')
plt.xlim(0, 1)
plt.legend(["Normalized (divided by 1050)","divided by 1024"])
plt.grid(True)
plt.show()
print("New coefficients:")
print(b2)