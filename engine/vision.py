import cv2
import numpy as np
import google.generativeai as genai
import imutils
from PIL import Image


def crop_board(image: Image) -> Image:
    """
    Crop the image to only include the board
    """
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    _, binary = cv2.threshold(gray, 15, 255, cv2.THRESH_BINARY | cv2.THRESH_OTSU)
    contours, _ = cv2.findContours(binary, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    max_contour = max(contours, key=cv2.contourArea)
    x, y, w, h = cv2.boundingRect(max_contour)
    cropped_image = image[y:y+h, x:x+w]

    cv2.imwrite('imgcrop/board_cropped.png', cropped_image)

    return cropped_image


def label_chessboard(board: Image | str) -> Image:
    if isinstance(board, str):
        board = cv2.imread(board)
    grid_size = 8
    square_height = board.shape[0] // grid_size
    square_width = board.shape[1] // grid_size

    for i in range(grid_size):
        for j in range(grid_size):
            label = f'{chr(65 + j)}{8 - i}'
            cv2.putText(
                board, label, (j * square_width + 30, (i + 1) * square_height - 30),
                cv2.FONT_HERSHEY_PLAIN, 0.4, (0, 0, 0), 1, cv2.LINE_AA
            )

    display_width = 800
    display_height = int(board.shape[0] * (display_width / board.shape[1]))

    cv2.imwrite('imglabel/board_labelled.png', board)

    return cv2.resize(board, (display_width, display_height))


def remove_bg_with_green_contours(image: Image) -> Image:
    hsv_image = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)
    green_low = np.array([40, 40, 40])
    green_high = np.array([80, 255, 255])
    kernel = np.ones((5, 5), np.uint8)
    mask = cv2.dilate(
        cv2.inRange(hsv_image, green_low, green_high), kernel, iterations=2
    )
    output = np.zeros_like(mask)
    output[:, :, :] = 255
    for i in range(3):
        output[:, :, i] = np.where(mask == 255, image[:, :, i], output[:, :, i])

    cv2.imwrite('imgcontours/board_contours.png', output)

    return output


def crop_moved_squares(image: Image) -> Image:
    """
    Crop the image to only include the green highlighted squares
    """
    hsv = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)
    green_low = np.array([40, 40, 40])
    green_high = np.array([80, 255, 255])
    kernel = np.ones((5, 5), np.uint8)
    mask = cv2.dilate(
       cv2.inRange(hsv, green_low, green_high), kernel, iterations=2
    )
    contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    cropped_images = []
    for contour in contours:
        if cv2.contourArea(contour) > 1000:  # Adjust this value as needed
            x, y, w, h = cv2.boundingRect(contour)
            cropped_image = image[y:y + h, x:x + w]
            cropped_images.append(cropped_image)

    if cropped_images:
        max_height = max(img.shape[0] for img in cropped_images)
        resized_images = [cv2.resize(img, (int(img.shape[1] * max_height / img.shape[0]), max_height)) for img in
                          cropped_images]

        cv2.imwrite('imgfinal/squares_cropped.png', np.hstack(resized_images))

        return np.hstack(resized_images)


def detect_move_made(prev_state: Image, curr_state: Image) -> list[str]:
    """
    Takes the previous and current state image and detects the piece moved on the board.
    """
    prev_state = cv2.resize(prev_state, (600, 360))
    curr_state = cv2.resize(curr_state, (600, 360))
    prev_gray = cv2.cvtColor(prev_state, cv2.COLOR_BGR2GRAY)
    curr_gray = cv2.cvtColor(curr_state, cv2.COLOR_BGR2GRAY)
    prev_blur = cv2.GaussianBlur(prev_gray, (5, 5), 0)
    curr_blur = cv2.GaussianBlur(curr_gray, (5, 5), 0)
    diff = cv2.absdiff(prev_blur, curr_blur)
    threshold = cv2.threshold(diff, 45, 255, cv2.THRESH_BINARY)[1]
    kernel = np.ones((6, 6), np.uint8)
    dilate = cv2.dilate(threshold, kernel, iterations=2)
    contours = imutils.grab_contours(
        cv2.findContours(dilate.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    )
    for contour in contours:
        if cv2.contourArea(contour) > 500:
            x, y, w, h = cv2.boundingRect(contour)
            cv2.rectangle(prev_state, (x, y), (x + w, y + h), (0, 0, 255), 2)
            cv2.rectangle(curr_state, (x, y), (x + w, y + h), (0, 255, 0), 2)

    cv2.imwrite('imgdiff/board_diff.png', curr_state)

    curr_state = crop_moved_squares(
        label_chessboard(
            remove_bg_with_green_contours(curr_state)
        )
    )
    model = genai.GenerativeModel('gemini-pro-vision')
    response = model.generate_content([curr_state, "Read the characters present inside the green boxes. List them in csv form."])
    return response.text.split(',')
