by Marius Versteegen, 2022

This folder contains some convenient classes that I use in some of my projects
with CleanRTOS.

Button     -  Objects of type Button monitor the state of a pin.
              Typically, the monitoring period is the same for all Buttons.
              Therefore, Buttons are not given a task for each of them.
              Instead, they are driven by a Handler that calls their update function
              periodically.

IButton    -  Button derives from IButton. You can use this interface to send commands
              to the button. For instance to register an object as a ButtonListener and 
              to call isButtonPressed().

IButtonListener - Tasks that are interested in the events of a Button should derive from
              IButtonListener. 

SharedOutputPin - SharedOutputPin is actually a combination of an OR operation and a pin.
              If one or more of the registered users "uses" the pin, the pin is set to 
              (remain-) logic 1. Otherwise, the pin is set to logic zero. 
              It is also possible to invert the logic by setting positiveLogic to false.

              Example of use: There are multiple servo-valves. To avoid vibration-noise 
              and power consumption, the servos are normally cut off from supply using 
              a mosfet connected to a pin.
              Thus if a thread likes to temporarily move a servo, all it needs to do is 
              to "use" the pin that powers the servo and "unuse" it afterwards.

ISharedOutputPin - SharedOutputPin derives from ISharedOutputPin. Via that interface, the 
              users of the SharedOutputPin communicate with it.

ISharedOutputPinUser - Users of a SharedOutputPin object should derive from this interface.
