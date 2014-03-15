

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

				var max=100,
					index=0,
					interval = $('#system_monitor_cpu').attr('interval'),
				 	data = new google.visualization.DataTable(),
         	options = {
						title: 'CPU Usage',
						width:200,
						height:200,
						vAxis: { 
							viewWindowMode:'explicit',
							viewWindow:{
								max:105,
								min:0,
							}
						}
        };

				data.addColumn( 'number', 'index');
				data.addColumn( 'number', 'Usage');
				for( var i=0; i< max; i++) {
					data.addRow([i,0]);
				}

        var chart = new google.visualization.LineChart(document.getElementById('system_monitor_cpu'));
        chart.draw(data, options);

				function tick() {
					console.log('tick', index, data.getNumberOfRows(), data.getNumberOfColumns());
					if ( index < max) {
						data.setValue( index++, 1, Math.random() * 100);
					}
					else {
						data.removeRow(0);
						data.addRow([index++,Math.random()*100]);
					}
					chart.draw(data, options);
				}
				setInterval( tick, interval * 1000);
			}

			// Load the Visualization API and the chart package.
			google.load("visualization", "1", {packages:["corechart"], "callback":drawChart});
			console.log('google api loaded');

				/*
			$('#run-test-ocs-button').bind('click', function(event) {
				$('.run-one-test-case-button').removeClass('fire').addClass( 'fire');
				$('.run-one-test-case-button[order=0]').trigger( 'click');
			});
			$('#add-action-button').bind('click', $.fn.addActionHandler);
			$('.run-one-test-case-button').bind('click', $.fn.runOneTestOcsHandler);
			$('.run-all-test-case-button').bind('click', function(event) {
				var target = $(event.target);
				var nid = $(target).attr('node');
				$('.run-one-test-case-button[node=' + nid +']').removeClass('fire').addClass('fire');
				$('.run-one-test-case-button[node=' + nid +'][tc=0]').trigger('click');
			});
			$('#clear-test-result-button').bind('click', function(event) {
				$('#run-test-ocs-result').text('');
			});

			$('.more_data').bind('click', function(event) {
				var target=$(event.target), 
					content=$(target).parent().children('.content'),
					print=$(target).attr('href'),
					json = $.parseJSON( $(content).text());
				if ( $(content).hasClass('export_result')) {
					json.data = $.parseJSON( json.data);
				}
				$(print).append( prettyPrint( json, {expanded:false}));
			});
			*/

			/*
			$('.export_result').each(function(i){
				var json = $.parseJSON($(this).text());
				json.data = $.parseJSON( json.data);
				console.log(json);
				//$(this).append( prettyPrint( json, {expanded:false}));
			});
			var str= $('.export_result').text();
			if ( str != 'undefined') {
				var json = $.parseJSON(str);
				console.log( json);
				$('.export_data').text( prettyPrint( json, {expanded:false}));
			}
			*/
		}
	};



})(jQuery);


