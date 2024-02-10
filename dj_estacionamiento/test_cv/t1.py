'''
Test Open cv
'''
import cv2
import pytesseract


imagen = cv2.imread("/Users/hernanr/myCode/Robotica/EstacionamientoESP32/dj_estacionamiento/media/vigilancia/picture3.jpg")  # leer imagenro
# Redimensionar la imagen para acelerar el procesamiento
imagen = cv2.resize(imagen, (600, 400), fx=0.5, fy=0.5)
gris = cv2.cvtColor(imagen, cv2.COLOR_BGR2GRAY)  # covertia a escala de grises
filtro = cv2.blur(
    gris, (2, 2), True
)  # disminuir ruido escala de grises con menos brillo
# cv2.imshow("filtro", filtro)
borde = cv2.Canny(
    filtro, 10, 40
)  # 10,40 detectar las lineas de bordes, umbral 150-200 (picture6(100-200))
# cv2.imshow("bordes", borde)
dilat = cv2.dilate(
    borde, (1, 1), iterations=1
)  # dilatacion de las lineas de bordes es igual a mas gruesas
# cv2.imshow("dilatacion", dilat)
contorno, _ = cv2.findContours(
    dilat, cv2.RETR_LIST, cv2.CHAIN_APPROX_SIMPLE
)  # (dilat, cv2.RETR_LIST, cv2.CHAIN_APPROX_SIMPLE) encontrar todos los contornos
# cv2.drawContours(imagen,contorno,-1,(255,0,0),2)
for c in contorno:  # verificar cada contorno
    area = cv2.contourArea(c)  # determinar area de cada contorno
    x, y, w, h = cv2.boundingRect(
        c
    )  # es obtienen los puntos del contorno para delimitando rectangulo
    epsilon = 0.06 * cv2.arcLength(c, True)
    aprox = cv2.approxPolyDP(
        c, epsilon, True
    )  # almacena el numero de vertices del contorno
    if (
        len(aprox) == 4 and area > 1000
    ):  # condicion de 4 vertices y area de placa mayor a 10000
        print("area=", area)
        ratio = float(w) / h  # ancho entre largo de la placa
        if ratio > 1.8:
            x, y, w, h = cv2.boundingRect(aprox)
            plac = imagen[y : y + h, x : x + w]
            cv2.imshow("recort a color", plac)
            pla = gris[
                y + 10 : y + h - 10, x + 10 : x + w - 5
            ]  # recorte de imagen de la placa en escala de grises
            cv2.imshow("recort", pla)
            height, width = pla.shape
            section_width = width // 7
            sections = []
            lectur = f""
            for i in range(7):
                start_x = i * section_width
                end_x = (i + 1) * section_width
                section = pla[:, start_x:end_x]
                sections.append(section)
                te = pytesseract.image_to_string(
                    section, config="--psm 8"
                )  # usar --psm 8
                for oc in te[0:1]:
                    lectur += oc + ""
            for i, section in enumerate(sections):
                cv2.imshow(f"seccion{i + 1}", section)
            print(f"lectura de placa: {lectur}")

            cv2.rectangle(
                imagen, (x, y), (x + w, y + h), (0, 255, 255), 4
            )  # rectangulo blaco de placa
            cv2.putText(
                imagen, lectur, (x + 20, y - 20), 1, 2.2, (0, 255, 255), 4
            )  # texto rojo de placa
            break
cv2.imshow("Vehiculo", imagen)  # imprimir en ventana la imagen original
cv2.waitKey(0)
