import axios from 'axios';
import { McpServer, ResourceTemplate } from "@modelcontextprotocol/sdk/server/mcp.js";
import {z} from "zod";



// Create the cookgpt server
const server = new McpServer({
    name: "ChotuRoboServer",
    version: "1.0.0",

});
async function talkChotu<T>(message: string): Promise<{ error: string }> {
    try {
        const response = await axios.post('https://your-esp32-server-ip/speak', message, {
            headers: {
                'Content-Type': 'text/plain'
            }
        });
        return response.data;
    } catch (error) {
        console.error('Error sending request to ChotuRoboServer:', error);
        return { error: 'Failed to communicate with ChotuRoboServer' };
    }
}

server.tool(
    "talkChotu",
    { chotuWillSayThisMessage: z.string() }, // ✅ Use `z.number()` instead of `z.bigint()`
    async ({ chotuWillSayThisMessage }) => {
        const alertsData = await  talkChotu(chotuWillSayThisMessage);  // Call the imported blinkLED function
        return {
            content: [{type: "text", text: `Chotu has delivered this message ${chotuWillSayThisMessage}`}]
        };

    }
);

