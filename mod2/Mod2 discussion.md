Based on your understanding of the reading and videos for this module, create and post your response to the discussion topic. Then per the syllabus, provide constructive comments on the other’s discussion posts.

At the site Arduino Motorized Roller Blinds [https://www.instructables.com/Arduino-Motorized-Roller-Blinds/]
, there is a good design for automating the opening and closing of window blinds using an Arduino with simple and cheap circuitry.  It would also be good to be able to open and close them via a web interface.

What would need to change in the design to also Help opening and closing selected blinds via a Web Interface?

---

Provided we have enough current to run both the arduino and stepper motor, we could use poe to power the arduino while giving us connectivity, saving us a cable or wifi configuration. The instructables guide shows us an IR control scheme, which can be retained, while a restful API can be started on the arduino, hosted by a simple web server, which can expose the controls necessary to operate the motors by web control using our simple API. This would be both efficient to implement, and add the requested functionality.