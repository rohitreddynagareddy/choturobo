import { McpServer, ResourceTemplate } from "@modelcontextprotocol/sdk/server/mcp.js";
import { StdioServerTransport } from "@modelcontextprotocol/sdk/server/stdio.js";
import { z } from "zod";

import {
    CallToolRequestSchema,
    CompleteRequestSchema,
    CreateMessageRequest,
    CreateMessageResultSchema,
    GetPromptRequestSchema,
    ListPromptsRequestSchema,
    ListResourcesRequestSchema,
    ListResourceTemplatesRequestSchema,
    ListToolsRequestSchema,
    ReadResourceRequestSchema,
    Resource,
    SetLevelRequestSchema,
    SubscribeRequestSchema,
    Tool,
    ToolSchema,
    UnsubscribeRequestSchema,
} from "@modelcontextprotocol/sdk/types.js";
import {zodToJsonSchema} from "zod-to-json-schema";


// Create the cookgpt server
const server = new McpServer({
    name: "ChotuRobo",
    version: "1.0.0",

});
import { Board, Led, Servo, Motor, Pin, Relay, Sensor } from "johnny-five";

// Initialize Board
const board = new Board();

// Declare peripherals globally
let led: Led | null = null;
let buzzer: Led | null = null;
let motor: Motor | null = null;
let servo: Servo | null = null;
let fan: Led | null = null;
let relay: Relay | null = null;
let temperatureSensor: Sensor | null = null;
let ultrasonicSensor: Sensor | null = null;

board.on("ready", () => {
    console.log("Board is ready!");

    // Initialize components
    led = new Led(10); // LED on pin 10
    buzzer = new Led(9); // Buzzer on pin 9
    motor = new Motor({
        pins: { pwm: 6, dir: 7 }, // PWM on pin 6, direction on pin 7
    });
    servo = new Servo(5); // Servo motor on pin 5
    fan = new Led(4); // Fan connected to pin 4
    relay = new Relay(3); // Relay module on pin 3
    temperatureSensor = new Sensor("A0"); // Temperature sensor on analog pin A0
    ultrasonicSensor = new Sensor("A1"); // Ultrasonic sensor on analog pin A1

    console.log("All peripherals initialized.");
});
// Define LED globally
const ToolInputSchema = ToolSchema.shape.inputSchema;

server.tool(
    "blinkLED",
    { time: z.number() }, // ✅ Use `z.number()` instead of `z.bigint()`
    async ({ time }) => {
        if (board.isReady && led) {
            led.blink(time);
            setTimeout(() => led!.stop().off(), 5000); // Stop blinking after 5 seconds
        }

            return {
                content: [{ type: "text", text: `I will blink the LED for ${time}` }]
            };

    }
);

server.tool(
    "buzz",
    { time: z.number() },
    async ({ time }) => {
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





