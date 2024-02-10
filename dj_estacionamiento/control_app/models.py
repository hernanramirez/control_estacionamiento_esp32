from django.db import models


class Foto(models.Model):
    """Modelo para registrar las fotos del estacionamiento."""
    foto = models.ImageField(upload_to="vigilancia", null=True, blank=True)
    created_at = models.DateTimeField(auto_now_add=True)
    updated_at = models.DateTimeField(auto_now=True)

    def __str__(self):
        return self.foto.name
    
    class Meta:
        verbose_name = "Foto"
        verbose_name_plural = "Fotos"


class Accesos(models.Model):
    """ Modelo para registrar los accesos al estacionamiento"""
    foto = models.OneToOneField(Foto, on_delete=models.CASCADE)
    placa = models.CharField(max_length=10)
    fecha = models.DateTimeField(auto_now_add=True)
    fecha_salida = models.DateTimeField(null=True, blank=True)
    created_at = models.DateTimeField(auto_now_add=True)
    updated_at = models.DateTimeField(auto_now=True)

    class Meta:
        verbose_name = "Acceso"
        verbose_name_plural = "Accesos"
