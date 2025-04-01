import { McpServer, ResourceTemplate } from "@modelcontextprotocol/sdk/server/mcp.js";
import { StdioServerTransport } from "@modelcontextprotocol/sdk/server/stdio.js";
import { z } from "zod";


// Create the cookgpt server
const server = new McpServer({
    name: "ChotuRoboServer",
    version: "1.0.0",

});
// Register available resources

server.tool(
    "moveBackward",
    { steps: z.number() }, // Takes the number of steps to move clockwise
    ({ steps }) => {
        fetch("http://192.168.2.116/clockwise", {
            method: "GET",
        }).then((response) => response.json())  // Process the API response if needed
            .catch((error) => {
                console.error("Error during moveForward request:", error);
            });

        return {
            content: [{ type: "text", text: `I am moving the motor clockwise for ${steps} steps.` }],
        };
    }
);

server.tool(
    "moveForward",
    { steps: z.number() }, // Takes the number of steps to move counterclockwise
    ({ steps }) => {
        fetch("http://192.168.2.116/counterclockwise", {
            method: "GET",
        }).then((response) => response.json())  // Process the API response if needed
            .catch((error) => {
                console.error("Error during moveBackward request:", error);
            });

        return {
            content: [{ type: "text", text: `I am moving the motor counterclockwise for ${steps} steps.` }],
        };
    }
);

server.tool(
    "makeChotuSpeak",
    { message: z.string() }, // Takes a string message to be spoken
    ({ message }) => {
        const encodedMessage = encodeURIComponent(message); // Encode the message for URL safety
        const url = `http://localhost:8080/sendAudioLocal?textToPlay=${encodedMessage}`;

        fetch(url, {
            method: "GET",
        })
            .then((response) => response.json()) // Process the API response if needed
            .catch((error) => {
                console.error("Error during makeChotuSpeak request:", error);
            });

        return {
            content: [{ type: "text", text: `Your message "${message}" has been delivered.` }],
        };
    }
);

server.tool(
    "makeChotuSing",
    { message: z.string() }, // Takes a string message to be spoken
    ({ message }) => {
        const encodedMessage = encodeURIComponent(message); // Encode the message for URL safety
        const url = `http://localhost:8080/singASong?actor=${encodedMessage}`;

        fetch(url, {
            method: "GET",
        })
            .then((response) => response.json()) // Process the API response if needed
            .catch((error) => {
                console.error("Error during makeChotuSpeak request:", error);
            });

        return {
            content: [{ type: "text", text: `Your message "${message}" has been delivered.` }],
        };
    }
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