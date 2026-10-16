console.log("IPC Chat script loaded");

const form = document.getElementById("chat-form");
const username = document.getElementById("username");
const messageInput = document.getElementById("message-input");
const messages = document.getElementById("messages");

form.addEventListener("submit", async function (event) {
    event.preventDefault();

    const name = username.value.trim();
    const text = messageInput.value.trim();

    if (!name || !text) return;

    try {
        const response = await fetch("http://localhost:8081/message", {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: JSON.stringify({
                name: name,
                message: text
            })
        });

        const result = await response.text();

        if (!response.ok) {
            throw new Error(result);
        }

        const welcome = messages.querySelector(".welcome");
        if (welcome) welcome.remove();

        const message = document.createElement("div");
        message.className = "message";

        const sender = document.createElement("strong");
        sender.textContent = name;

        const content = document.createElement("div");
        content.textContent = text;

        message.append(sender, content);
        messages.appendChild(message);
        messages.scrollTop = messages.scrollHeight;

        messageInput.value = "";
        messageInput.focus();
        console.log("Backend response:", result);

    } catch (error) {
        alert("Backend connection failed: " + error.message);
    }
});
