import { deleteExpense } from './api.js';
import { renderDashboard } from './dashboard.js';

export function renderTable(expenses) {
  const tableBody = document.querySelector('#expenses-table tbody');
  tableBody.innerHTML = ''; // Clear previous data

  expenses.forEach((expense) => {
    const row = document.createElement('tr');

    row.innerHTML = `
      <td>${expense.id}</td>
      <td>${expense.amount}</td>
      <td>${expense.category}</td>
      <td>${expense.note}</td>
      <td>${expense.date}</td>
      <td>
        <!-- ✅ Emoji or plain text Delete button -->
        <button class="delete-btn" data-id="${expense.id}">🗑️ Delete</button>
        
        <!-- 🛠️ Optional: Uncomment if you later add edit logic -->
        <!-- <button class="edit-btn" data-id="${expense.id}">✏️ Edit</button> -->
      </td>
    `;

    tableBody.appendChild(row);
  });

  // ✅ Bind delete button events
  document.querySelectorAll(".delete-btn").forEach(button => {
    button.addEventListener("click", async () => {
      const id = button.dataset.id;
      const token = localStorage.getItem("token");

      if (confirm("Delete this expense?")) {
        try {
          await deleteExpense(token, id);
          alert("Deleted!");
          renderDashboard(); // Refresh data after deletion
        } catch (error) {
          alert("Failed to delete expense.");
          console.error(error);
        }
      }
    });
  });
}

export function renderAnalytics(analytics) {
  const container = document.getElementById("analytics");
  container.innerHTML = "";

  if (!analytics || Object.keys(analytics).length === 0) {
    container.innerHTML = "<p>No analytics available.</p>";
    return;
  }

  const list = document.createElement("ul");
  for (const [category, total] of Object.entries(analytics)) {
    const item = document.createElement("li");
    item.textContent = `${category}: $${parseFloat(total).toFixed(2)}`;
    list.appendChild(item);
  }

  container.appendChild(list);
}
