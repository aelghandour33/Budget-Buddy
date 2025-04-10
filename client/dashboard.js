// dashboard.js
import { fetchExpenses, addExpense, fetchAnalytics } from './api.js';
import { renderTable, renderAnalytics } from './ui.js';

export function renderDashboard() {
  const token = localStorage.getItem('token');

  if (!token) {
    alert("No user logged in");
    return;
  }

  // ✅ Hide login and show dashboard
  document.getElementById("auth-section").classList.add("hidden");
  document.getElementById("dashboard-section").classList.remove("hidden");

  // ✅ Display username from JWT
  try {
    const payload = JSON.parse(atob(token.split('.')[1]));
    document.getElementById("display-username").textContent = payload.username || "User";
  } catch (e) {
    console.warn("Invalid token format:", e);
  }

  // ✅ Fetch and render expenses
  fetchExpenses(token)
    .then(expenses => renderTable(expenses))
    .catch(error => console.error("Failed to load expenses:", error));

  // ✅ Fetch and render analytics
  fetchAnalytics(token)
    .then(analytics => renderAnalytics(analytics))
    .catch(error => console.error("Failed to load analytics:", error));

  // ✅ Add new expense handler
  const expenseForm = document.getElementById("expense-form");
  expenseForm.addEventListener("submit", event => {
    event.preventDefault();

    const amount = document.getElementById("amount").value;
    const category = document.getElementById("category").value;
    const note = document.getElementById("note").value;
    const date = document.getElementById("date").value;

    const expenseData = { amount, category, note, date };

    addExpense(token, expenseData)
      .then(response => {
        alert("Expense added successfully!");
        renderDashboard(); // refresh view instead of reload
      })
      .catch(error => {
        alert("Failed to add expense.");
        console.error(error);
      });
  });

  // ✅ Logout
  document.getElementById("logout-btn").addEventListener("click", () => {
    localStorage.removeItem("token");
    location.reload();
  });
}
