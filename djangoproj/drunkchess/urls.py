from django.urls import path
from . import views

urlpatterns = [
    path('', views.board, name='board'),
    path('leaderboard/',views.leaders,name='leaders'),
    path('cell-clicked-first/<int:col>/<int:row>/', views.cell_clicked_first, name='cell-clicked-first'),
    path('move/<int:startX>/<int:startY>/<int:endX>/<int:endY>/', views.move, name='move'),
    path('start-game/<int:difficulty>/<str:side>/', views.start_game, name='start_game'),
    path('reset-idle/',views.reset_idle,name='reset_idle'),
    path('set-params/',views.set_params,name="set_params"),
    path('move-enemy/',views.move_enemy,name="move_enemy"),
    path('save-score/',views.save_result,name="save_result"),
    path('dump/',views.dump,name="dump"),
    path('upload/',views.upload,name="upload"),
    path('stop/',views.stop_client,name="stop_client"),
    path('set-substitute/',views.set_substitute,name='set_substitute')
]
