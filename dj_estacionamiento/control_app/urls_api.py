
from django.urls import include, path
from rest_framework import routers

from control_app.api.views import AccesosViewSet, FotoViewSet

router = routers.DefaultRouter()
router.register(r"fotos", FotoViewSet)  # El nombre de la ruta es opcional, pero es una buena práctica
router.register(r"accesos", AccesosViewSet)

urlpatterns = [
    path("", include(router.urls)),
]
