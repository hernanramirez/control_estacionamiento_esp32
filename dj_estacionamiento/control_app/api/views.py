
from rest_framework import viewsets
from rest_framework.parsers import JSONParser, MultiPartParser

from control_app.api.serializers import FotoSerializer, AccesosSerializer
from control_app.models import Foto, Accesos


class FotoViewSet(viewsets.ModelViewSet):
    queryset = Foto.objects.all()
    serializer_class = FotoSerializer
    parser_classes = (MultiPartParser, JSONParser)


class AccesosViewSet(viewsets.ModelViewSet):
    queryset = Accesos.objects.all()
    serializer_class = AccesosSerializer
    parser_classes = (MultiPartParser, JSONParser)
    lookup_field = 'foto'
