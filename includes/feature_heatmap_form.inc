<?php

function feature_heatmap_form($form, &$form_state){
  $form['descriptions'] = array(
    '#type' => 'item',
    '#markup' => t('<b>If the feature unique names is not empty, it will search by feature unique names.
                   otherwise, it will search by feature IDs.</b>')
  );

  $form['heatmap_feature_uniquename'] = array(
    '#type' => 'textarea',
    '#title' => t('Enter feature unique names'),
    '#description' => t('<b>Example feature unique names:</b> Juglans_nigra_120313_comp34229_c0_seq1 ,Quercus_rubra_120313_comp31392_c0_seq1, Acer_saccharum_022416_comp57075_c0_seq8, Liquidambar_styraciflua_01052015_comp34573_c0_seq2,Fraxinus_pennsylvanica_120313_comp50845_c0_seq2, Nyssa_sylvatica_10132014_comp27824_c0_seq1, Quercus_rubra_120313_comp13209_c0_seq1'),
    '#attributes' => array(
      'placeholder' => t('Nyssa_sylvatica_10132014_comp23154_c0_seq12_m.8255, Quercus_rubra_120313_comp172643_c0_seq1_m.37896, Liriodendron_tulipifera_10132014_comp16064_c0_seq1'),
    ),
  );

  $form['heatmap_feature_id'] = array(
    '#type' => 'textarea',
    '#title' => t('Enter feature IDs'),
    '#description' => t('<b>Example feature IDs:</b> 618978, 669963, 384973, 518858,1889963,1869369,2599817,1890195,352534, 299910, 372333,2519176,1884325, 374334, 210144, 574110,1862036'),
    '#attributes' => array(
      'placeholder' => t('1678806, 1625926, 298564, 200410')
    ),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Display Expression Heatmap'),
  );

  return $form;
}


/*
 * feature_heatmap_form submit handler
 */
function feature_heatmap_form_submit($form, &$form_state){
  $sql_feature_id = "SELECT ER.elementresult_id, ER.signal, F.feature_id, F.uniquename, B.name, B.biomaterial_id, B.description FROM {chado.elementresult} ER
           INNER JOIN {chado.element} E ON E.element_id = ER.element_id
           INNER JOIN {chado.feature} F ON F.feature_id = E.feature_id
           INNER JOIN {chado.quantification} Q ON Q.quantification_id = ER.quantification_id
           INNER JOIN {chado.acquisition} AQ ON AQ.acquisition_id = Q.acquisition_id
           INNER JOIN {chado.assay} A ON A.assay_id = AQ.assay_id
           INNER JOIN {chado.assay_biomaterial} AB ON AB.assay_id = A.assay_id
           INNER JOIN {chado.biomaterial} B ON B.biomaterial_id = AB.biomaterial_id
         WHERE
           F.feature_id = :feature_id";

  $sql_feature_uniquename = "SELECT ER.elementresult_id, ER.signal, F.feature_id, F.uniquename, B.name, B.biomaterial_id, B.description FROM {chado.elementresult} ER
           INNER JOIN {chado.element} E ON E.element_id = ER.element_id
           INNER JOIN {chado.feature} F ON F.feature_id = E.feature_id
           INNER JOIN {chado.quantification} Q ON Q.quantification_id = ER.quantification_id
           INNER JOIN {chado.acquisition} AQ ON AQ.acquisition_id = Q.acquisition_id
           INNER JOIN {chado.assay} A ON A.assay_id = AQ.assay_id
           INNER JOIN {chado.assay_biomaterial} AB ON AB.assay_id = A.assay_id
           INNER JOIN {chado.biomaterial} B ON B.biomaterial_id = AB.biomaterial_id
         WHERE
           F.uniquename = :feature_uniquename";

  $expressions = array();
  $heatmap_feature_id = $form_state['values']['heatmap_feature_id'];
  $heatmap_feature_uniquename = $form_state['values']['heatmap_feature_uniquename'];


  if(!empty($heatmap_feature_uniquename)) {  // search with feature uniquename
    $feature_ids = explode(',', $heatmap_feature_uniquename);
    foreach($feature_ids as $feature_id){
      // trim white space at the beginning and the end
      $feature_id = trim($feature_id);
      $expressions[$feature_id] = db_query($sql_feature_uniquename, array(":feature_uniquename" => $feature_id))->fetchAllAssoc('elementresult_id');
    }
  } else { //search with feature id
    $feature_ids = explode(',', $heatmap_feature_id);
    foreach($feature_ids as $feature_id){
      // trim white space at the beginning and the end
      $feature_id = (int)trim($feature_id); // coerce feature_id into integer. Otherwise it will generate an error. 
                                            //I think this may be because the feature_id field in the chado.feature table only store int values.
      $expressions[$feature_id] = db_query($sql_feature_id, array(":feature_id" => $feature_id))->fetchAllAssoc('elementresult_id');
    }
  }


  if(!empty($expressions)){
    // Get all biomaterial names from different organisms
    $biomaterials = array();
    foreach($expressions as $features){
      // each feature may have may have multiple expression data from different treatment
      foreach($features as $feature){
        $biomaterials[$feature->name] = $feature->name;
      }
    }
  
    // Loop through all biomaterial so that each feature have all biomaterials (treatment)
    // Assign NULL as expression intensity to all treatment.
    $expression_data = array();
    foreach($expressions as $feature_id=>$features){
      foreach($biomaterials as $biomaterial_id){
            $expression_data[$feature_id][$biomaterial_id] = NULL;
      }   
    }

    // Foreach feature, loop through the biomaterials it has and get the signal value (expression data)
    foreach($expressions as $feature_id=>$features){
      foreach($features as $feature){
            $expression_data[$feature_id][$feature->name] = (float)$feature->signal;
      }
    }
  
    $y = array();
    $z = array();
    foreach($expression_data as $feature_id=>$values){
      $y[] = 'ID:'.$feature_id;
      $x = array_keys($values);
      $z[] = array_values($values); 
    }
 
    // Build variable for plotly function in js file.
    // Note that this variable requires that the x, y, z and type are not within double quotations
    // To build this variable, we first create a string, and then use javascripts' eval() function
    // to convert this string into a javascript variable.
    // The eval() function is called in the heatmap.js file. 
    if(!empty($x) and !empty($y) and !empty($x)){
      $_SESSION['heatmap_data'] = '[{'.'
                                      x:'.json_encode($x).', 
                                      y:'.json_encode($y).', 
                                      z:'.json_encode($z).', 
                                      type:"heatmap"}]';
    } else {
      $_SESSION['heatmap_data'] = '';
    }
  }

  /*
  $_SESSION['heatmap_x'] = $x;
  $_SESSION['heatmap_y'] = $y;
  $_SESSION['heatmap_z'] = $z;
  */
}  
