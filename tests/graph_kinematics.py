import matplotlib.pyplot as plt
import numpy as np

from kinematics import coords_from_angles as get_coords
from kinematics import angles_from_coords as get_angles


t = np.arange(0, 50, 1)
x, y = 200, 200

theta_bg = np.zeros((len(t), 1))
theta_ba = np.zeros((len(t), 1))
theta_aa = np.zeros((len(t), 1))

for i in range(len(t)):
    theta_bg[i], theta_ba[i], theta_aa[i] = get_angles(x, y, t[i], degrees=True)

fig, (ax1, ax2, ax3) = plt.subplots(3, 1)
ax1.plot(t, theta_bg)
ax1.set_title('Base angle vs z')
ax1.xlabel = 'z'

ax2.plot(t, theta_ba)
ax2.set_title('Arm 1 angle vs z')
ax2.xlabel = 'z'

ax3.plot(t, theta_aa)
ax3.set_title('Inter-arm angle vs z')
ax3.xlabel = 'z'

plt.show()

