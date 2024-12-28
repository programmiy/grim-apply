import sys
import os
import cv2
import numpy as np
import random
import re
from PyQt5.QtWidgets import (
    QApplication, QMainWindow, QLabel, QLineEdit, QPushButton, QFileDialog
)
from PyQt5.QtGui import QPixmap, QImage
from PyQt5.QtCore import Qt, QTimer

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.initUI() # UI를 초기화하려면 필수적으로 호출해야 하는 함수

    def initUI(self):
        self.setWindowTitle("Image Dialog using pyqt5")
        self.setGeometry(100, 100, 1280, 720)

        # 입력과 입력창의 레이블 설정
        self.label_x1 = QLabel("x1:", self)
        self.label_x1.setGeometry(20, 20, 50, 30)
        self.input_x1 = QLineEdit(self)
        self.input_x1.setGeometry(70, 20, 100, 30)

        self.label_y1 = QLabel("y1:", self)
        self.label_y1.setGeometry(200, 20, 50, 30)
        self.input_y1 = QLineEdit(self)
        self.input_y1.setGeometry(250, 20, 100, 30)

        self.label_x2 = QLabel("x2:", self)
        self.label_x2.setGeometry(20, 60, 50, 30)
        self.input_x2 = QLineEdit(self)
        self.input_x2.setGeometry(70, 60, 100, 30)

        self.label_y2 = QLabel("y2:", self)
        self.label_y2.setGeometry(200, 60, 50, 30)
        self.input_y2 = QLineEdit(self)
        self.input_y2.setGeometry(250, 60, 100, 30)

        # 버튼 설정
        self.draw_button = QPushButton("Draw", self)
        self.draw_button.setGeometry(400, 20, 100, 30)
        self.draw_button.clicked.connect(self.draw_circle)

        self.action_button = QPushButton("Action", self)
        self.action_button.setGeometry(520, 20, 100, 30)
        self.action_button.clicked.connect(self.animate_circle)

        self.open_button = QPushButton("Open", self)
        self.open_button.setGeometry(640, 20, 100, 30)
        self.open_button.clicked.connect(self.open_image)

        # Image display label, 일종의 캔버스 역할
        self.image_label = QLabel(self)
        self.image_label.setGeometry(20, 120, 1216, 540)
        self.image_label.setStyleSheet("border: 1px solid black; margin-left: 0px; margin-right: 0px;")
        self.image_label.setAlignment(Qt.AlignCenter)
        
        # 하얀 바탕의 QPixmap 제작, Image display label을 draw_circle 함수에서 사용하기 위해 미리 생성
        white_pixmap = QPixmap(1216, 540)
        white_pixmap.fill(Qt.white)
        self.image_label.setPixmap(white_pixmap)
            
        # 사용하는 변수
        self.timer = QTimer() # pyqt5의 QTimer 사용
        self.timer.timeout.connect(self.move_circle) # move_circle 함수 제한시간 설정
        self.steps = []
        self.current_step = 0
        self.radius = random.randint(30, 70)
        
        # 폴더가 없으면 생성, 이미지가 있으면 삭제
        if not os.path.exists("image"):
            os.makedirs("image")
        elif os.path.exists("image"): # 재실행시 기존 저장 이미지 삭제
            files = os.listdir("image")
            for file in files:
                os.remove("image/" + file)
                
                
    def qimage_to_cv2(self, qimg):
        qimg = qimg.convertToFormat(QImage.Format_RGB888)  # 888 > 256, 256, 256을 의미
        width = qimg.width()
        height = qimg.height()
        ptr = qimg.bits() # pointer를 사용하여 메모리를 직접 접근
        ptr.setsize(height * width * 3)
        img = np.array(ptr).reshape(height, width, 3)
        return img
    
    
    def draw_circle(self):
        try: # 숫자 값이 아닌 값을 입력시 에러 방지
            x1, y1 = int(self.input_x1.text()), int(self.input_y1.text())
            x1, y1 = x1+20, y1+120
            pixmap = self.image_label.pixmap()
            # QPixmap을 QImage로 변환
            qimg = pixmap.toImage()

            # QImage를 OpenCV 이미지로 변환 >> shape를 구하기 위해
            img = self.qimage_to_cv2(qimg)
            height, width, _ = img.shape
            proportional_radius = min(width, height) // 20  # 이미지 크기의 1/20로 설정

            # self.radius와 proportional_radius를 조합하여 random_radius 설정, 랜덤값을 조정하기 힘들어서 추가
            random_radius = (self.radius + proportional_radius) // 2  # 두 값의 평균

            
            # 위에 지정한 img와는 다르게 원을 그릴 img 초기화
            img = 255 * np.ones((720, 1280, 3), dtype=np.uint8)
            
            cv2.circle(img, (x1, y1), random_radius, (0, 0, 0), -1)
            
            self.display_image(img, x1, y1)
        except ValueError:
            self.statusBar().showMessage("Invalid input for coordinates.")

    def animate_circle(self):
        try:
            x1, y1 = int(self.input_x1.text()), int(self.input_y1.text())
            x2, y2 = int(self.input_x2.text()), int(self.input_y2.text())
            # x1, y1 = x1+20, y1+120
            # x2, y2 = x2+20, y2+120 >> move에서 반영
            dx = (x2 - x1) / 50
            dy = (y2 - y1) / 50
            # >> 50으로 나누어 1씩 증가하도록 설정, 아래 코드에서 50번 이동하는 반복문 설정
            self.steps = [(int(x1 + i * dx), int(y1 + i * dy)) for i in range(51)] 
            self.current_step = 0
            self.timer.start(200)  # x2, y2까지 200ms = 0.2s 안에 이동하게 설정
        except ValueError:
            self.statusBar().showMessage("Invalid input for coordinates.")

    def move_circle(self):
        if self.current_step >= len(self.steps):
            self.timer.stop() # 50번 이동했으면 멈춤
            return

        x, y = self.steps[self.current_step]
        x, y = x+20, y+120
        
        pixmap = self.image_label.pixmap()
        # QPixmap을 QImage로 변환
        qimg = pixmap.toImage()

        # QImage를 OpenCV 이미지로 변환 >> shape를 구하기 위해
        img = self.qimage_to_cv2(qimg)
        height, width, _ = img.shape
        
        proportional_radius = min(width, height) // 20  # 이미지 크기의 1/20로 설정
        
        # 기존 원을 지우기 위해 이미지를 초기 상태로 되돌림
        img = 255 * np.ones((720, 1280, 3), dtype=np.uint8)
          
        # self.radius와 proportional_radius를 조합하여 random_radius 설정
        random_radius = (self.radius + proportional_radius) // 2  # 두 값의 평균
        cv2.circle(img, (x, y), random_radius, (0, 0, 0), -1)

        # jpg 이미지 저장
        
        filepath = os.path.join("image", f"frame_{x-20}_{y-120}.jpg")
        cv2.imwrite(filepath, img)

        
        
        self.display_image(img, x, y)
        self.current_step += 1
        
        
    def display_image(self, img, x=None, y=None):
        height, width, channel = img.shape # channel은 사용하지는 않지만, 3개의 변수를 내보내는 shape이므로 사용
        bytes_per_line = 3 * width
        qimg = QImage(img.data, width, height, bytes_per_line, QImage.Format_RGB888).rgbSwapped()
        pixmap = QPixmap.fromImage(qimg)
        
        self.image_label.setPixmap(pixmap)
 

    def open_image(self):
        options = QFileDialog.Options()
        file_path, _ = QFileDialog.getOpenFileName(self, "Open Image File", "", "Image Files (*.bmp *.jpg *.jpeg)", options=options)
        # *.bmp *.jpg *.jpeg 파일만 열 수 있도록 설정
        
        if file_path:
            img = cv2.imdecode(np.fromfile(file_path, dtype=np.uint8), cv2.IMREAD_COLOR)
            
            if img is not None:
                search = re.search(r"frame_(\d+)_(\d+)", file_path) # frame_x_y.jpg 파일명에서 x, y값을 추출
                if search:
                    x = int(search.group(1))  
                    y = int(search.group(2))  
                    
                    cv2.drawMarker(img, (x+20, y+120), (0, 0, 255), markerType=cv2.MARKER_TILTED_CROSS, thickness=2)
                    cv2.putText(img, f"({x}, {y})", (x + 20 + self.radius, y +120 - self.radius), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 0), 1, cv2.LINE_AA)
                    self.statusBar().showMessage("x, y값"f"{x}, {y}")
                    
                    x,y = x+20, y+120
                    self.display_image(img, x, y)
                else:
                    self.statusBar().showMessage("Filename does not search the expected pattern.")
            else:
                self.statusBar().showMessage("Failed to load image.")

if __name__ == "__main__":
    app = QApplication(sys.argv)
    mainWin = MainWindow()
    mainWin.show()
    sys.exit(app.exec())