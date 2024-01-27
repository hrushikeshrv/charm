import random
import math


L1 = 200
L2 = 160


def angles_from_coords(x, y, z, degrees=True):
    theta_bb = math.atan(y / x)

    num = x**2 + y**2 + z**2 - L1**2 - L2**2
    den = -2 * L1 * L2

    print(num)
    print(den)
    print(num / den)
    theta_aa = math.acos(num / den)

    a = L1 * math.cos(theta_bb) - L2 * math.cos(theta_aa) * math.cos(theta_bb)
    b = L2 * math.sin(theta_aa) * math.cos(theta_bb)

    den = 2*(a**2+b**2)
    num1 = 2*a*x + math.sqrt(4*a*a*x*x - 4*(a**2+b**2)*(x**2-b**2))
    num2 = 2*a*x - math.sqrt(4*a*a*x*x - 4*(a**2+b**2)*(x**2-b**2))

    theta_ba1 = math.acos(num1 / den)
    theta_ba2 = math.acos(num2 / den)

    print(f'Theta BB: {theta_bb}')
    print(f'Theta BA 1: {theta_ba1}')
    print(f'Theta BA 2: {theta_ba2}')
    print(f'Theta AA: {theta_aa}')

    if degrees:
        return math.degrees(theta_bb), math.degrees(theta_ba1), math.degrees(theta_aa)
    return theta_bb, theta_ba1, theta_aa


def coords_from_angles(theta_bb, theta_ba, theta_aa):
    x = (L1 * math.cos(theta_ba) - L2 * math.cos(theta_aa + theta_ba)) * math.cos(theta_bb)
    y = (L1 * math.cos(theta_ba) - L2 * math.cos(theta_aa + theta_ba)) * math.sin(theta_bb)
    z = L1 * math.sin(theta_ba) - L2 * math.sin(theta_aa + theta_ba)
    return x, y, z


def get_random_coords():
    x = math.floor(random.random() * L1 - L1//2)
    y = math.floor(random.random() * L1)
    z = math.floor(random.random() * 60)
    return x, y, z


if __name__ == '__main__':
    keep_going = True
    while keep_going:
        random_input = input('Choose random coordinates? (y/n): ').strip().lower().startswith('y')
        if random_input:
            x, y, z = get_random_coords()
        else:
            x = float(input('Enter the x coordinate: '))
            y = float(input('Enter the y coordinate: '))
            z = float(input('Enter the z coordinate: '))
        print(f'Chose coordinates: ({x}, {y}, {z})')
        theta_bb, theta_ba, theta_aa = angles_from_coords(x, y, z)
        print(f'Got angles theta_bb: {theta_bb}, theta_ba: {theta_ba}, theta_aa: {theta_aa}')
        rev_x, rev_y, rev_z = coords_from_angles(theta_bb, theta_ba, theta_aa)
        print(f'Got back coordinates: ({x}, {y}, {z})\n')
        
        keep_going = input('Repeat? (y/n): ').strip().lower().startswith('y')

