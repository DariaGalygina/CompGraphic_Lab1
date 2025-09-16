import math
import tkinter as tk
from tkinter import ttk
from turtle import color

class FunctionPlotter:
    def __init__(self, root):
        self.root = root
        self.root.title("Лабораторная 1: графики функций")
        self.root.geometry("900x700")
        self.root.configure(bg="#293e50")
        
        self.x_min = -10.0
        self.x_max = 10.0
        self.function_var = tk.StringVar(value="sin(x)")
        
        self.colors = {
            "bg": "#a17e50",
            "control_bg": "#34495e",
            "text": "#ecf0f1",
            "accent": "#374c3c",
            "graph": "#e74c3c",
            "axes": "#bdc3c7",
            "canvas_bg": "#1a2530",
            "button_bg": "#3498db"
        }
        
        # Интерфейс
        self.create_widgets()
        
        # График
        self.plot_function()
    
    def create_widgets(self):
        # Фрейм
        control_frame = ttk.Frame(self.root, padding="15")
        control_frame.pack(fill=tk.X, pady=(10, 5))
        
        style = ttk.Style()
        style.configure("TFrame", background=self.colors["control_bg"])
        style.configure("TLabel", background=self.colors["control_bg"], foreground=self.colors["text"], font=("Arial", 10, "bold"))
        style.configure("TButton", background=self.colors["button_bg"], font=("Arial", 10, "bold"))
        
        ttk.Label(control_frame, text="Функция:").grid(row=0, column=0, sticky=tk.W, padx=(0, 5))
        functions = ["sin(x)", "x^2", "cos(x)", "exp(x)", "log(x)", "x^3", "sin(x)*cos(2x)"]
        function_combo = ttk.Combobox(control_frame, textvariable=self.function_var, values=functions, state="readonly", width=15, font=("Arial", 10))
        function_combo.grid(row=0, column=1, padx=5)
        function_combo.bind("<<ComboboxSelected>>", lambda e: self.plot_function())
        
        # Диапазон X
        ttk.Label(control_frame, text="X min:").grid(row=0, column=2, padx=(20,0))
        self.x_min_var = tk.StringVar(value=str(self.x_min))
        x_min_entry = ttk.Entry(control_frame, textvariable=self.x_min_var, width=8, font=("Arial", 10))
        x_min_entry.grid(row=0, column=3)
        x_min_entry.bind("<Return>", lambda e: self.update_range())
        
        ttk.Label(control_frame, text="X max:").grid(row=0, column=4, padx=(5,0))
        self.x_max_var = tk.StringVar(value=str(self.x_max))
        x_max_entry = ttk.Entry(control_frame, textvariable=self.x_max_var, width=8, font=("Arial", 10))
        x_max_entry.grid(row=0, column=5)
        x_max_entry.bind("<Return>", lambda e: self.update_range())
        
        ttk.Button(control_frame, text="Обновить", 
                  command=self.update_range).grid(row=0, column=6, padx=15)
        
        # Холст 
        self.canvas = tk.Canvas(self.root, bg=self.colors["canvas_bg"], highlightthickness=0, relief="raised", bd=2)
        self.canvas.pack(fill=tk.BOTH, expand=True, padx=15, pady=(0, 15))
        
        # Изменение размера
        self.canvas.bind("<Configure>", lambda e: self.plot_function())
    
    # Обновляет диапазон и перерисовывает график
    def update_range(self):
        
        try:
            new_x_min = float(self.x_min_var.get())
            new_x_max = float(self.x_max_var.get())
            
            if new_x_min >= new_x_max:
                raise ValueError("Минимум должен быть меньше максимума")
                
            self.x_min = new_x_min
            self.x_max = new_x_max
            self.plot_function()
            
        except ValueError as e:
            # Восстанавливаем предыдущие значения
            self.x_min_var.set(str(self.x_min))
            self.x_max_var.set(str(self.x_max))
    
    # Возвращает функцию по её имени
    def get_function(self, func_name):
        if func_name == "sin(x)":
            return math.sin
        elif func_name == "x^2":
            return lambda x: x**2
        elif func_name == "cos(x)":
            return math.cos
        elif func_name == "exp(x)":
            return math.exp
        elif func_name == "log(x)":
            return lambda x: math.log(x) if x > 0 else float('nan')
        elif func_name == "x^3":
            return lambda x: x**3
        elif func_name == "sin(x)*cos(2x)":
            return lambda x: math.sin(x) * math.cos(2*x)
        else:
            return math.sin
    
    # Строит график функции
    def plot_function(self):
        if not hasattr(self, 'canvas'):
            return
            
        self.canvas.delete("all")
        
        width = self.canvas.winfo_width()
        height = self.canvas.winfo_height()
        
        if width <= 1 or height <= 1:
            return
        
        # Функция
        func = self.get_function(self.function_var.get())
        
        points = []
        valid_points = []
        
        # Количество точек для построения
        num_points = min(width, 1000)
        
        for i in range(num_points):
            x = self.x_min + (self.x_max - self.x_min) * i / (num_points - 1)
            try:
                y = func(x)
                if not math.isnan(y) and not math.isinf(y):
                    points.append((x, y))
                    valid_points.append(y)
            except (ValueError, ZeroDivisionError):
                continue
        
        if not points:
            return
        
        # min и max Y для масштабирования
        y_min = min(valid_points)
        y_max = max(valid_points)
        
        # Отступ
        y_range = y_max - y_min
        if y_range == 0:
            y_range = 1
        y_min -= y_range * 0.1
        y_max += y_range * 0.1
        
        # Преобразование координат
        canvas_points = []
        for x, y in points:
            canvas_x = (x - self.x_min) / (self.x_max - self.x_min) * (width - 60) + 30
            canvas_y = height - 30 - (y - y_min) / (y_max - y_min) * (height - 60)
            canvas_points.append((canvas_x, canvas_y))
        
        # Оси
        self.draw_axes(width, height, self.x_min, self.x_max, y_min, y_max)
        
        # График 
        if len(canvas_points) > 1:
            for i in range(len(canvas_points) - 1):
                x1, y1 = canvas_points[i]
                x2, y2 = canvas_points[i + 1]
                self.canvas.create_line(x1, y1, x2, y2, fill=self.colors["graph"], width=3, smooth=True)
        
        # Доп инфа о функции
        info_text = f"{self.function_var.get()}   |   x ∈ [{self.x_min:.2f}, {self.x_max:.2f}]"
        self.canvas.create_text(160, 3, text=info_text, fill=self.colors["text"], font=("Arial", 12, "bold"), anchor=tk.N)
    
    # Рисует координатные оси
    def draw_axes(self, width, height, x_min, x_max, y_min, y_max):
        # Ось Y
        y_axis_x = 30
        if x_min <= 0 <= x_max:
            y_axis_x = 30 + abs(x_min) / (x_max - x_min) * (width - 60)
        self.canvas.create_line(y_axis_x, 30, y_axis_x, height - 30, fill=self.colors["axes"], width=2)
        
        # Ось X
        x_axis_y = height - 30
        if y_min <= 0 <= y_max:
            x_axis_y = height - 30 - abs(y_min) / (y_max - y_min) * (height - 60)
        self.canvas.create_line(30, x_axis_y, width - 30, x_axis_y, fill=self.colors["axes"], width=2)
        
        # Подписи 
        self.canvas.create_text(width - 15, x_axis_y - 15, text="x", fill=self.colors["text"], font=("Arial", 12, "bold"), anchor=tk.SE)
        self.canvas.create_text(y_axis_x + 15, 15, text="y", fill=self.colors["text"], font=("Arial", 12, "bold"), anchor=tk.NW)
        
        # Разметка 
        self.draw_axis_ticks(width, height, x_min, x_max, y_min, y_max, y_axis_x, x_axis_y)
    
    # Рисует разметку на осях
    def draw_axis_ticks(self, width, height, x_min, x_max, y_min, y_max, y_axis_x, x_axis_y):
        
        # Ось X
        x_step = max(1, (x_max - x_min) / 10)
        x = math.ceil(x_min / x_step) * x_step
        while x <= x_max:
            if x_min <= x <= x_max:
                x_pos = 30 + (x - x_min) / (x_max - x_min) * (width - 60)
                self.canvas.create_line(x_pos, x_axis_y - 5, x_pos, x_axis_y + 5, fill=self.colors["axes"], width=2)
                self.canvas.create_text(x_pos, x_axis_y + 15, text=f"{x:.1f}", anchor=tk.N, fill=self.colors["text"], font=("Arial", 9))
            x += x_step
        
        # Ось Y
        y_step = max(0.1, (y_max - y_min) / 10)
        y = math.ceil(y_min / y_step) * y_step
        while y <= y_max:
            if y_min <= y <= y_max:
                y_pos = height - 30 - (y - y_min) / (y_max - y_min) * (height - 60)
                self.canvas.create_line(y_axis_x - 5, y_pos, y_axis_x + 5, y_pos, fill=self.colors["axes"], width=2)
                self.canvas.create_text(y_axis_x - 10, y_pos, text=f"{y:.1f}", anchor=tk.E, fill=self.colors["text"], font=("Arial", 9))
            y += y_step

def main():
    root = tk.Tk()
    app = FunctionPlotter(root)
    root.mainloop()

if __name__ == "__main__":
    main()