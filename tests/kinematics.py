import random
import math


l1 = 400
l2 = 400


def angles_from_coords(x, y, z):
    if x != 0:
        theta_bg = math.atan(y/x)
        if theta_bg < 0:
            theta_bg = math.pi + theta_bg
    else:
        theta_bg = math.pi / 2

    den = 2 * l1 * math.sqrt(x**2 + y**2 + z**2)
    num = l1**2 + x**2 + y**2 + z**2 - l2**2

    theta_ba = math.acos(num/den) + math.atan(z/math.sqrt(x**2 + y**2))

    num = l1**2 - x**2 - y**2 - z**2 + l2**2
    den = 2 * l1 * l2

    theta_aa = math.acos(num/den)

    return theta_bg, theta_ba, theta_aa


def coords_from_angles(theta_bg, theta_ba, theta_aa):
    z = l1 * math.sin(theta_ba) - l2 * math.cos(theta_aa + theta_ba - math.pi/2)
    rxy = l1 * math.cos(theta_ba) + l2 * math.sin(theta_aa + theta_ba - math.pi/2)
    x = rxy * math.cos(theta_bg)
    y = rxy * math.sin(theta_bg)
    
    return x, y, z


def get_random_coords():
    x = math.floor(random.random() * 400 - 200)
    y = math.floor(random.random() * 400)
    z = math.floor(random.random() * 20)
    return x, y, z

for i in range(10):
    x, y, z = get_random_coords()
    print(f'Chose random coordinates: ({x}, {y}, {z})')
    theta_bg, theta_ba, theta_aa = angles_from_coords(x, y, z)
    print(f'Got angles theta_bg = {math.degrees(theta_bg)}, theta_ba'\
            f' = {math.degrees(theta_ba)}, theta_aa = {math.degrees(theta_aa)}')
    rev_x, rev_y, rev_z = coords_from_angles(theta_bg, theta_ba, theta_aa)
    print(f'Got back coordinates: ({rev_x}, {rev_y}, {rev_z})\n')

