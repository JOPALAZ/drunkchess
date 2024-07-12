from django.urls import path
from . import views

urlpatterns = [
    path('', views.board, name='board'),
    path('cell-clicked-first/<int:col>/<int:row>/', views.cell_clicked_first, name='cell-clicked-first'),
    path('move/<int:startX>/<int:startY>/<int:endX>/<int:endY>/', views.move, name='move'),
    path('start-game/<int:difficulty>/<str:side>/', views.start_game, name='start_game'),
]
