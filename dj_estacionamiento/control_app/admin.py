from django.contrib import admin

# Register your models here.
from django.contrib import admin

from control_app.models import Foto, Accesos


@admin.register(Foto)
class VigilanciaAdmin(admin.ModelAdmin):
    list_display = ("id", "foto", "created_at", "updated_at")
    search_fields = ("id", "foto", "created_at", "updated_at")
    list_filter = ("id", "foto", "created_at", "updated_at")
    readonly_fields = ("created_at", "updated_at")

@admin.register(Accesos)
class AccesosAdmin(admin.ModelAdmin):
    pass
    list_display = ("id", "foto_id", "foto", "placa", "fecha", "fecha_salida", "created_at", "updated_at")
    # search_fields = ("id", "foto", "placa", "fecha", "fecha_salida", "created_at", "updated_at")
    # list_filter = ("id", "foto", "placa", "fecha", "fecha_salida", "created_at", "updated_at")
    # readonly_fields = ("created_at", "updated_at")