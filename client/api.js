// api.js

const API_URL = "http://127.0.0.1:5004";

// Fetch user data (for login)
export async function fetchUserData(username, password) {
  try {
    const response = await fetch(`${API_URL}/login`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ username, password }),
    });

    if (!response.ok) throw new Error("Login failed");

    const data = await response.json();
    return data;
  } catch (error) {
    console.error("Error fetching user data:", error);
    throw error;
  }
}

// Fetch expenses
export async function fetchExpenses(token) {
  try {
    const response = await fetch(`${API_URL}/expenses`, {
      method: 'GET',
      headers: { 'Authorization': `Bearer ${token}` }
    });

    if (!response.ok) throw new Error("Failed to fetch expenses");

    const data = await response.json();
    return data.expenses;
  } catch (error) {
    console.error("Error fetching expenses:", error);
    throw error;
  }
}

// Add expense
export async function addExpense(token, expense) {
  try {
    const response = await fetch(`${API_URL}/expenses`, {
      method: 'POST',
      headers: {
        'Authorization': `Bearer ${token}`,
        'Content-Type': 'application/json'
      },
      body: JSON.stringify(expense)
    });

    if (!response.ok) throw new Error("Failed to add expense");

    const data = await response.json();
    return data;
  } catch (error) {
    console.error("Error adding expense:", error);
    throw error;
  }
}

// Fetch analytics
export async function fetchAnalytics(token) {
  try {
    const response = await fetch(`${API_URL}/analytics`, {
      method: 'GET',
      headers: { 'Authorization': `Bearer ${token}` }
    });

    if (!response.ok) throw new Error("Failed to fetch analytics");

    const data = await response.json();
    return data;
  } catch (error) {
    console.error("Error fetching analytics:", error);
    throw error;
  }
}
// Delete expense
export async function deleteExpense(token, expenseId) {
    try {
      const response = await fetch(`http://localhost:5004/expenses`, {
        method: 'DELETE',
        headers: {
          'Authorization': `Bearer ${token}`,
          'Content-Type': 'application/json'
        },
        body: JSON.stringify({ id: expenseId })
      });
  
      if (!response.ok) throw new Error("Failed to delete expense");
  
      return await response.json();
    } catch (error) {
      console.error("Error deleting expense:", error);
      throw error;
    }
  }
  