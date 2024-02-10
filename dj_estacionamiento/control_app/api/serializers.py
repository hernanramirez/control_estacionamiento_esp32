from drf_extra_fields.fields import Base64ImageField
from rest_framework import serializers

from control_app.models import Foto, Accesos


class FotoSerializer(serializers.ModelSerializer):
    """
    Serializer class for the Foto model.
    """
    foto = Base64ImageField(required=False)

    class Meta:
        """
        The Meta class provides metadata options for the Foto serializer.
        """
        model = Foto
        fields = "__all__"

    def create(self, validated_data):
        instance = super().create(validated_data)
        return {'id': instance.id}


class AccesosSerializer(serializers.ModelSerializer):
    '''Serializer para el modelo Accesos.'''
    foto = FotoSerializer()

    class Meta:
        model = Accesos
        fields = "__all__"
