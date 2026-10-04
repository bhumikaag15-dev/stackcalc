
const expressionInput = document.getElementById("expression");
const resultDisplay = document.getElementById("result");
const postfixDisplay = document.getElementById("postfix");
const message = document.getElementById("message");
const historyList = document.getElementById("historyList");

let history = [];

function showMessage(text, type = "") {
    message.textContent = text;
    message.className = "message " + type;
}

function insertValue(value) {
    const start = expressionInput.selectionStart;
    const end = expressionInput.selectionEnd;
    const current = expressionInput.value;

    expressionInput.value =
        current.slice(0, start) + value + current.slice(end);

    const newPosition = start + value.length;

    expressionInput.focus();
    expressionInput.setSelectionRange(newPosition, newPosition);

    showMessage("Expression updated.");
}

function clearCalculator() {
    expressionInput.value = "";
    resultDisplay.textContent = "0";
    postfixDisplay.textContent =
        "Your postfix expression will appear here";

    showMessage("Calculator cleared.");
    expressionInput.focus();
}

function deleteLast() {
    const start = expressionInput.selectionStart;
    const end = expressionInput.selectionEnd;
    const current = expressionInput.value;

    if (start !== end) {
        expressionInput.value =
            current.slice(0, start) + current.slice(end);

        expressionInput.setSelectionRange(start, start);
    }
    else if (start > 0) {
        expressionInput.value =
            current.slice(0, start - 1) + current.slice(end);

        expressionInput.setSelectionRange(start - 1, start - 1);
    }

    expressionInput.focus();
}

async function calculate() {
    const expression = expressionInput.value.trim();

    if (!expression) {
        showMessage("Please enter an expression.", "error");
        return;
    }

    const calculateButton = document.getElementById("calculate");

    calculateButton.disabled = true;
    calculateButton.textContent = "CALCULATING...";

    showMessage("Evaluating using C++ stack...");

    try {
        const response = await fetch("/api/evaluate", {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: JSON.stringify({ expression })
        });

        const data = await response.json();

        if (!response.ok) {
            throw new Error(data.error || "Calculation failed.");
        }

        resultDisplay.textContent = data.result;
        postfixDisplay.textContent = data.postfix;

        showMessage("Expression evaluated successfully!", "success");

        addToHistory(expression, data.result, data.postfix);

    } catch (error) {
        resultDisplay.textContent = "Error";
        postfixDisplay.textContent = "Unable to evaluate expression.";

        showMessage(error.message, "error");

    } finally {
        calculateButton.disabled = false;
        calculateButton.textContent = "= EVALUATE EXPRESSION";
    }
}

function addToHistory(expression, result, postfix) {
    history.unshift({
        expression,
        result,
        postfix
    });

    history = history.slice(0, 10);

    renderHistory();
}

function renderHistory() {
    historyList.innerHTML = "";

    if (history.length === 0) {
        historyList.innerHTML = `
            <div class="empty-history">
                <div class="empty-icon">↻</div>
                <p>No calculations yet</p>
                <small>Your results will appear here.</small>
            </div>
        `;
        return;
    }

    history.forEach(item => {
        const div = document.createElement("div");

        div.className = "history-item";

        const expression = document.createElement("div");
        expression.className = "history-expression";
        expression.textContent = item.expression;

        const result = document.createElement("div");
        result.className = "history-result";
        result.textContent = "= " + item.result;

        div.appendChild(expression);
        div.appendChild(result);

        div.addEventListener("click", () => {
            expressionInput.value = item.expression;
            resultDisplay.textContent = item.result;
            postfixDisplay.textContent = item.postfix;
            showMessage("Previous calculation loaded.");
        });

        historyList.appendChild(div);
    });
}

// Calculator buttons

document.querySelectorAll("[data-value]").forEach(button => {

    button.addEventListener("click", () => {
        insertValue(button.dataset.value);
    });

});

document.querySelector('[data-action="clear"]')
    .addEventListener("click", clearCalculator);

document.querySelector('[data-action="delete"]')
    .addEventListener("click", deleteLast);

document.getElementById("calculate")
    .addEventListener("click", calculate);

document.getElementById("clearHistory")
    .addEventListener("click", () => {
        history = [];
        renderHistory();
        showMessage("History cleared.");
    });

// Keyboard support

expressionInput.addEventListener("keydown", event => {

    if (event.key === "Enter") {
        event.preventDefault();
        calculate();
    }

    if (event.key === "Escape") {
        clearCalculator();
    }

});