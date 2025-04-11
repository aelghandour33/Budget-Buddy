// dashboard.js
import { fetchExpenses, addExpense, fetchAnalytics } from './api.js';
import { renderTable, renderAnalytics } from './ui.js';

let formListenerAttached = false;

export function renderDashboard() {
  const token = localStorage.getItem('token');
  if (!token) {
    alert("No user logged in");
    return;
  }

  // Hide login and show dashboard
  document.getElementById("auth-section").classList.add("hidden");
  document.getElementById("dashboard-section").classList.remove("hidden");

  //  Display username from JWT
  try {
    const payload = JSON.parse(atob(token.split('.')[1]));
    document.getElementById("display-username").textContent = payload.username || "User";
  } catch (e) {
    console.warn("Invalid token format:", e);
  }

  //  Fetch and render expenses and analytics
  fetchExpenses(token).then(renderTable).catch(console.error);
  fetchAnalytics(token).then(renderAnalytics).catch(console.error);

  //  Attach submit handler only once
  if (!formListenerAttached) {
    const expenseForm = document.getElementById("expense-form");
    expenseForm.addEventListener("submit", event => {
      event.preventDefault();

      const amount = document.getElementById("amount").value;
      const category = document.getElementById("category").value;
      const note = document.getElementById("note").value;
      const date = document.getElementById("date").value;

      const expenseData = { amount, category, note, date };

      addExpense(token, expenseData)
        .then(() => {
          alert("Expense added!");
          renderDashboard(); // re-render view
        })
        .catch(error => {
          alert("Failed to add expense.");
          console.error(error);
        });
    });

    formListenerAttached = true; //  Prevent duplicate listeners
  }

  // Logout
  document.getElementById("logout-btn").onclick = () => {
    localStorage.removeItem("token");
    location.reload();
  };
}
