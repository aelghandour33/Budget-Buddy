// ✅ auth.js
// Handles login and stores JWT token
export async function login(username, password) {
    const response = await fetch("http://localhost:5004/login", {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify({ username, password })
    });
  
    const data = await response.json();
    if (data.status === "success" && data.token) {
      localStorage.setItem("token", data.token);
      return { success: true };
    } else {
      return { success: false, message: data.message || "Login failed" };
    }
  }
  
  export function getToken() {
    return localStorage.getItem("token");
  }
  
  export function logout() {
    localStorage.removeItem("token");
  }
  