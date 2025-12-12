function toHex(v) {
  const h = Number(v).toString(16).toUpperCase();
  return h.length === 1 ? "0" + h : h;
}

function hexToRgb(hex) {
  if (!hex || hex[0] !== "#") return { r: 0, g: 0, b: 0 };

  if (hex.length === 4) {
    const r = hex[1] + hex[1];
    const g = hex[2] + hex[2];
    const b = hex[3] + hex[3];
    return {
      r: parseInt(r, 16),
      g: parseInt(g, 16),
      b: parseInt(b, 16),
    };
  }

  if (hex.length === 7) {
    return {
      r: parseInt(hex.substr(1, 2), 16),
      g: parseInt(hex.substr(3, 2), 16),
      b: parseInt(hex.substr(5, 2), 16),
    };
  }

  return { r: 0, g: 0, b: 0 };
}

function updatePreviewFromSliders() {
  const r = document.getElementById("r").value;
  const g = document.getElementById("g").value;
  const b = document.getElementById("b").value;

  document.getElementById("val-r").textContent = r;
  document.getElementById("val-g").textContent = g;
  document.getElementById("val-b").textContent = b;

  const rgb = `rgb(${r}, ${g}, ${b})`;
  const preview = document.getElementById("preview");
  preview.style.background = rgb;
  preview.style.boxShadow = `0 0 30px rgba(${r},${g},${b},0.7)`;

  const hex = "#" + toHex(r) + toHex(g) + toHex(b);
  const hexEl = document.getElementById("hex");
  if (hexEl.textContent !== hex) {
    hexEl.textContent = hex;
  }

  const picker = document.getElementById("colorPicker");
  if (picker && picker.value.toUpperCase() !== hex) {
    picker.value = hex;
  }
}

function updateSlidersFromRgb(r, g, b) {
  document.getElementById("r").value = r;
  document.getElementById("g").value = g;
  document.getElementById("b").value = b;
  updatePreviewFromSliders();
}

function updateSlidersFromHex(hex) {
  const { r, g, b } = hexToRgb(hex);
  updateSlidersFromRgb(r, g, b);
}

function sendColor(r, g, b) {
  const status = document.getElementById("status");
  status.textContent = "Sending...";

  fetch(`/set?r=${r}&g=${g}&b=${b}`)
    .then((res) => res.text())
    .then((text) => {
      status.textContent = text.trim() || "OK";
    })
    .catch(() => {
      status.textContent = "Connection error";
    });
}

function loadInitialColor() {
  fetch("/get")
    .then((res) => res.json())
    .then((data) => {
      if (
        typeof data.r === "number" &&
        typeof data.g === "number" &&
        typeof data.b === "number"
      ) {
        updateSlidersFromRgb(data.r, data.g, data.b);
      } else {
        updatePreviewFromSliders();
      }
    })
    .catch(() => {
      document.getElementById("status").textContent =
        "Failed to retrieve state";
    });
}

document.addEventListener("DOMContentLoaded", function () {
  ["r", "g", "b"].forEach((id) => {
    document.getElementById(id).addEventListener("input", () => {
      updatePreviewFromSliders();
    });
  });

  const picker = document.getElementById("colorPicker");
  if (picker) {
    picker.addEventListener("input", (e) => {
      updateSlidersFromHex(e.target.value);
    });
  }

  document.getElementById("apply").addEventListener("click", (e) => {
    e.preventDefault();
    const r = document.getElementById("r").value;
    const g = document.getElementById("g").value;
    const b = document.getElementById("b").value;
    sendColor(r, g, b);
  });

  document.getElementById("off").addEventListener("click", (e) => {
    e.preventDefault();
    updateSlidersFromRgb(0, 0, 0);
    sendColor(0, 0, 0);
  });

  loadInitialColor();
});
