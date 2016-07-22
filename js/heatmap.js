(function($){
  Drupal.behaviors.heatmap = {
    attach: function(context, settings){
      $(document).ready(function(){
        var data = [
          {
            z: [[1, 20, 30, 50, 1], [20, 1, 60, 80, 30], [30, 60, 1, -10, 20]],
            x: ['Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday'],
            y: ['Morning', 'Afternoon', 'Evening'],
            type: 'heatmap'
          }
        ];
        
        Plotly.newPlot('vis_expression', data);
      })
    }
  }
})(jQuery);
