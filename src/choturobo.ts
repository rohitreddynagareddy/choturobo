import { McpServer, ResourceTemplate } from "@modelcontextprotocol/sdk/server/mcp.js";
import { StdioServerTransport } from "@modelcontextprotocol/sdk/server/stdio.js";
import { z } from "zod";
import pkg from 'johnny-five';
const { Board, Led, Servo, Motor, Relay, Sensor } = pkg;


// Create the cookgpt server
const server = new McpServer({
    name: "ChotuRoboServer",
    version: "1.0.0",

});
// Register available resources





async function blinkLED<T>(time: number): Promise<T | null> {

    const board = new Board({
        debug: false, //
    });

    board.on("ready", () => {
        const led = new Led(10); // Create LED on pin 10

        // Blink the LED for the specified time
         led.blink(time);

        // Stop blinking after 5 seconds
        setTimeout(() => {
            led.stop().off(); // Stop and turn off the LED
            return ("done") as T;
        }, 5000);
    });

    board.on("error", (error) => {
        return "error on board"// Reject the Promise if there is an error
    });
    return ("done") as T;
}

server.tool(
    "blinkLED",
    { time: z.number() }, // ✅ Use `z.number()` instead of `z.bigint()`
    async ({ time }) => {
        const alertsData = await  blinkLED(time);  // Call the imported blinkLED function
          return {
                content: [{type: "text", text: `I am blinking the LED for ${time}`}]
            };

    }
);

server.tool(
    "buzz",
    { time: z.number() },
    async ({ time }) => {
        const board = new Board({
            debug: false, //
        });
       const buzzer = new Led(9);
        if (board.isReady && buzzer) {
            buzzer.on();
            setTimeout(() => buzzer!.off(), time);
        }
        return {
            content: [{ type: "text", text: `Buzzer will sound for ${time} milliseconds.` }]
        };
    }
);

server.tool(
    "runMotor",
    { speed: z.number() },
    async ({ speed }) => {
        const board = new Board({
            debug: false, //
        });
        const motor = new Motor({
            pins: { pwm: 6, dir: 7 }, // PWM on pin 6, direction on pin 7
        });

        if (board.isReady && motor) {
            motor.start(speed);
            setTimeout(() => motor!.stop(), 5000); // Stop motor after 5 seconds
        }
        return {
            content: [{ type: "text", text: `Motor running at speed ${speed}.` }]
        };
    }
);

server.tool(
    "moveServo",
    { angle: z.number() },
    async ({ angle }) => {
        const board = new Board({
            debug: false, //
        });
       const servo = new Servo(5); // Servo motor on pin 5

        if (board.isReady && servo) {
            servo.to(angle);
        }
        return {
            content: [{ type: "text", text: `Servo moved to ${angle} degrees.` }]
        };
    }
);

server.tool(
    "controlFan",
    { state: z.boolean() },
    async ({ state }) => {

        const board = new Board({
            debug: false, //
        });
        const  fan = new Led(4); // Fan connected to pin 4

        if (board.isReady && fan) {
            state ? fan.on() : fan!.off();
        }
        return {
            content: [{ type: "text", text: `Fan turned ${state ? "on" : "off"}.` }]
        };
    }
);

server.tool(
    "toggleRelay",
    { state: z.boolean() },
    async ({ state }) => {
        const board = new Board({
            debug: false, //
        });
        const relay = new Relay(3); // Relay module on pin 3

        if (board.isReady && relay) {
            state ? relay.open() : relay.close();
        }
        return {
            content: [{ type: "text", text: `Relay switched ${state ? "ON" : "OFF"}.` }]
        };
    }
);

server.tool(
    "readTemperature",
    {},
    async () => {
        const board = new Board({
            debug: false, //
        });
        const temperatureSensor = new Sensor("A0"); // Temperature sensor on analog pin A0

        if (board.isReady && temperatureSensor) {
            const temp = temperatureSensor.value; // Read from sensor
            return {
                content: [{ type: "text", text: `Current temperature: ${temp}°C.` }]
            };
        }
        return {
            content: [{ type: "text", text: "Temperature sensor not available." }]
        };
    }
);

server.tool(
    "readDistance",
    {},
    async () => {
        const board = new Board({
            debug: false, //
        });
        const ultrasonicSensor = new Sensor("A1"); // Ultrasonic sensor on analog pin A1
        if (board.isReady && ultrasonicSensor) {
            const distance = ultrasonicSensor.raw; // Read distance in cm
            return {
                content: [{ type: "text", text: `Detected object at ${distance} cm.` }]
            };
        }
        return {
            content: [{ type: "text", text: "Ultrasonic sensor not available." }]
        };
    }
);
server.prompt(
    "move-chotu",
    { steps: z.string() },
    ({ steps }) => ({
        messages: [{
            role: "user",
            content: {
                type: "text",
                text: `I will move chotu this any steps :\n\n${steps}`
            }
        }]
    })
);

server.prompt(
    "start-chotu",
    {},
    () => ({
        messages: [{
            role: "user",
            content: {
                type: "text",
                text: "Initializing Chotu and getting ready to operate."
            }
        }]
    })
);

server.prompt(
    "stop-chotu",
    {},
    () => ({
        messages: [{
            role: "user",
            content: {
                type: "text",
                text: "Stopping Chotu safely and shutting down operations."
            }
        }]
    })
);

server.prompt(
    "turn-chotu",
    { direction: z.enum(["left", "right"]) },
    ({ direction }) => ({
        messages: [{
            role: "user",
            content: {
                type: "text",
                text: `Turning Chotu to the ${direction}.`
            }
        }]
    })
);

server.prompt(
    "set-chotu-speed",
    { speed: z.string() },
    ({ speed }) => ({
        messages: [{
            role: "user",
            content: {
                type: "text",
                text: `Setting Chotu's speed to ${speed}.`
            }
        }]
    })
);
async function main() {
    const transport = new StdioServerTransport();
    await server.connect(transport);
    console.error("Chotu Robo Running");
}

main().catch((error) => {
    console.error("Fatal error in main():", error);
    process.exit(1);
});