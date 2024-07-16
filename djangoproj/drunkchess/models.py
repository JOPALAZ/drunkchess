from django.db import models

# Create your models here.
class Item (models.Model):
    moves=models.IntegerField()
    name=models.CharField(max_length=256)
    def __str__(self) -> str:
        return f'{self.name}:{self.moves}'