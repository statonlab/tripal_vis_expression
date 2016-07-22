(function($){
  Drupal.behaviors.tripal_vis_expression = {
    attach: function(context, settings){
      $(document).ready(function(){
        /*
        var x = settings.tripal_vis_expression.heatmap_x;
        var y = settings.tripal_vis_expression.heatmap_y;
        var z = settings.tripal_vis_expression.heatmap_z;
        var type = 'heatmap';
        var heatmap_data = [{x, y, z, type}];
        */
        var heatmap_data = eval(settings.tripal_vis_expression.heatmap_data); 
        Plotly.newPlot('vis_expression', heatmap_data);
      })
    }
  }
})(jQuery);
