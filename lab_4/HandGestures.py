import Leap
import time
import serial


def main():

    # setup serial port that writes to bluetooth
    s = serial.Serial('COM4', 115200, timeout=1)
    time.sleep(2)
    print 'connected ', s.isOpen()
    s.flush()

    # leap motion initialization
    controller = Leap.Controller()
    controller.set_policy(Leap.Controller.POLICY_BACKGROUND_FRAMES)
    controller.set_policy(Leap.Controller.POLICY_IMAGES)
    controller.set_policy(Leap.Controller.POLICY_OPTIMIZE_HMD)

    # wait for leap to connect
    while not controller.is_connected:
        continue

    # last command
    last = 'not started'
    while controller.is_connected: # main loop
        time.sleep(0.2) # delay
        frame = controller.frame() # get frame
        if frame.is_valid:
            hands = frame.hands
            if not hands.is_empty:
                hand = hands.frontmost # get the hand
                if hand.confidence >= 0.90:
                    if hand.sphere_radius < 50: # close fist = stop
                        last = 's'
                    elif hand.palm_normal.x < -0.5: # palm facing right = right
                        last = 'r'
                    elif hand.palm_normal.x > 0.5: # palm facing left = left
                        last = 'l'
                    elif hand.palm_normal.y > 0.5: # palm facing back = reverse/back
                        last = 'b'
                    else:
                        last = 'f' # forward for everything else
            # write and flush the buffer
            s.write(last + '\n')
            s.flush()
    s.close()


if __name__ == '__main__':
    main()