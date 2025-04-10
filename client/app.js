// app.js
import { fetchUserData } from './api.js';
import { renderDashboard } from './dashboard.js'; // ✅ Import moved to top level

const loginForm = document.getElementById("login-form");
loginForm.addEventListener("submit", function(event) {
  event.preventDefault();

  const username = document.getElementById("username").value;
  const password = document.getElementById("password").value;

  fetchUserData(username, password)
    .then(data => {
      if (data.status === "success") {
        // Store the token in localStorage
        localStorage.setItem("token", data.token);
        // ✅ Show the dashboard UI
        renderDashboard();
      } else {
        document.getElementById("login-error").textContent = data.message;
      }
    })
    .catch(error => {
      console.error("Error logging in:", error);
      document.getElementById("login-error").textContent = "Login failed!";
    });
});
