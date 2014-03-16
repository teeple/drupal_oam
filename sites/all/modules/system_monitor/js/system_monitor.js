
(function($) {
	Drupal.behaviors.system_monitor = {
		attach : function(context, settings) {
			console.log('system_monitor.js loaded');
			
			// Callback that creates and populates a data table,
			// instantiates the pie chart, passes in the data and
			// draws it.
			function drawChart() {
				console.log('draw chart');
				var chartList = {
				                 'cpu_load':{
				                	 id:'system_monitor_cpu_load',
				                	 columns:['index','usr','nice','sys','load'],
				                	 initval:[0,0,0,0],
				                	 vmax:105,
				                 },
				                 'cpu_loadavg':{
				                	 id:'system_monitor_cpu_loadavg',
				                	 columns:['index','1min','5min','15min'],
				                	 initval:[0,0,0],
				                 }
				};
				var interval = 3;
				
				// prepare data
				for( var i in chartList) {
					var c = chartList[i],
						obj = $('#'+c.id);
					c.max = c.index = 100;
					interval = $(obj).attr('interval'); 
					
					// build data
					c.data = new google.visualization.DataTable();
					for( var j=0; j< c.columns.length; j++) {
						c.data.addColumn('number',c.columns[j]);
					}
					for ( var j=0; j< c.max; j++) {
						c.data.addRow([j].concat(c.initval));
					}
					
					// build options
					c.options = {
							width : $(obj).attr('width'),
							height : $(obj).attr('height'),
							/*
							vAxis : {
								viewWindowMode : 'explicit',
								viewWindow : {
									max : 105,
									min : 0,
								}
							}
							*/
					};
					
					// draw initial chart
					c.chart = new google.visualization.LineChart(document
							.getElementById(c.id));
					c.chart.draw( c.data, c.options);
				}
				console.log(chartList);

				function drawChartWithData(data) {
					for( var i in data) {
						var c=chartList[i];
						c.data.removeRow(0);
						c.data.addRow([c.index++].concat(data[i].value));
						c.chart.draw( c.data, c.options);
					}
				}

				function tick() {
					
					var req = $.ajax({
						  type: "GET",
						  url: "/drupal_oam/ajax/mon/cpu_load",
						  dataType: "json",
						  complete:function(msg) {
							  console.log(msg);
							  var rsp=$.parseJSON(msg.response);
							  drawChartWithData(rsp.data);
							  /*
							   * 
							   data.removeRow(0);
							  data.addRow([index++].concat(rsp.data.value));
							  chart.draw(data, options);
							  */
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
