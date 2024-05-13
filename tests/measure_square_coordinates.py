import serial

socket = serial.Serial('COM3', baudrate=115200)

curr_square = 'A1'
curr_pos = 'hover'      # Can be "hover" or "lower"


def get_square_index(square):
    return (ord(square[0]) - ord('A')) * 8 + int(square[1]) - 1


def get_angles():
    socket.write('ready'.encode())
    return socket.readline().decode('utf-8').strip()[:-1]


def write_angle(angle, square, hover):
    idx = get_square_index(square)
    overwrite = False
    if not hover:
        if len(lower_angles[idx].strip()) > 0:
            resp = input(f'Overwrite previous angle? ({lower_angles[idx]}) - ').strip().lower()
            if resp == '':
                resp = 'y'
            overwrite = resp.startswith('y')
        else:
            overwrite = True

        if overwrite:
            with open('lower_angles_autogen.txt', 'w') as f:
                    lower_angles[idx] = '{' + angle.strip() + '}'
                    f.write('\n'.join(lower_angles) + '\n')
    else:
        if len(hover_angles[idx].strip()) > 0:
            resp = input(f'Overwrite previous angle? ({hover_angles[idx]}) - ').strip().lower()
            if resp == '':
                resp = 'y'
            overwrite = resp.startswith('y')
        else:
            overwrite = True

        if overwrite:
            with open('hover_angles_autogen.txt', 'w') as f:
                hover_angles[idx] = '{' + angle.strip() + '}'
                f.write('\n'.join(hover_angles) + '\n')


def refresh_angles():
    with open('lower_angles_autogen.txt', 'r') as f:
        lower_angles = list(map(lambda x: x.strip(), f.readlines()))

    with open('hover_angles_autogen.txt', 'r') as f:
        hover_angles = list(map(lambda x: x.strip(), f.readlines()))
    return lower_angles, hover_angles


def print_angles():
    print('\nHover angles ------------------')
    result = ''
    for i in range(8):
        result += ','.join(hover_angles[8*i:8*i+8]) + ',\n'
    print(result)
    print('\nLower angles ------------------')
    result = ''
    for i in range(8):
        result += ','.join(lower_angles[8*i:8*i+8]) + ',\n'
    print(result)


if __name__ == '__main__':
    lower_angles, hover_angles = refresh_angles()
    print_angles()
    
    while True:
        print(f'\nCurrent square - {curr_square}, current mode - {curr_pos}')
        
        action = input('Enter next action (Enter to capture coordinates, square name to change square, "hover" or "lower" to change mode, "print" to print current captured angles) - ')
        if action == '':
            angles = get_angles()
            write_angle(angles, curr_square, hover=curr_pos=="hover")
            if curr_pos == 'hover':
                curr_pos = 'lower'
            print(f'Captured angles - {angles}')
        
        elif action in ['hover', 'lower']:
            curr_pos = action
        elif action == 'print':
            print_angles()
        elif action == 'q':
            break
        else:
            curr_square = action
            curr_pos = 'hover'