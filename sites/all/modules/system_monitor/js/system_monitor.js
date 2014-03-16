
(function($) {
	Drupal.behaviors.system_monitor = {
		attach : function(context, settings) {
			console.log('system_monitor.js loaded');
			$('#system_monitor_cpu').append('CPU Usage');

			// Callback that creates and populates a data table,
			// instantiates the pie chart, passes in the data and
			// draws it.
			function drawChart() {
				console.log('draw chart');

				var max = 100, index = max, interval = $('#system_monitor_cpu')
						.attr('interval'), data = new google.visualization.DataTable(), options = {
					title : 'CPU Usage',
					width : 200,
					height : 200,
					vAxis : {
						viewWindowMode : 'explicit',
						viewWindow : {
							max : 105,
							min : 0,
						}
					}
				};

				data.addColumn('number', 'index');
				data.addColumn('number', 'usr');
				data.addColumn('number', 'nice');
				data.addColumn('number', 'sys');
				data.addColumn('number', 'idle');
				for ( var i = 0; i < max; i++) {
					data.addRow([i,0,0,0,0]);
				}

				var chart = new google.visualization.LineChart(document
						.getElementById('system_monitor_cpu'));
				chart.draw(data, options);

				function tick() {
					console.log('tick', index, data.getNumberOfRows(), data
							.getNumberOfColumns());
					
					var req = $.ajax({
						  type: "GET",
						  url: "/drupal_oam/ajax/mon/cpu_load",
						  dataType: "json",
						  complete:function(msg) {
							  console.log(msg);
							  var rsp=$.parseJSON(msg.response);
							  data.removeRow(0);
							  data.addRow([index++].concat(rsp.data.value));
							  chart.draw(data, options);
						  }
						});
				}
				setInterval(tick, interval * 1000);
			}

			// Load the Visualization API and the chart package.
			google.load("visualization", "1", {
				packages : [ "corechart" ],
				"callback" : drawChart
			});
			console.log('google api loaded');

		}
	};

})(jQuery);
