<?php

function feature_heatmap_form($form, &$form_state){
  $form['heatmap_feature_id'] = array(
    '#type' => 'textarea',
    '#title' => t('Enter feature IDs'),
    '#attributes' => array(
      'placeholder' => t('1678806, 1625926, 298564, 200410')
    ),
  );

  $form['heatmap_feature_name'] = array(
    '#type' => 'textarea',
    '#title' => t('Enter feature unique names'),
    '#attributes' => array(
      'placeholder' => t('Nyssa_sylvatica_10132014_comp23154_c0_seq12_m.8255, Quercus_rubra_120313_comp172643_c0_seq1_m.37896, Liriodendron_tulipifera_10132014_comp16064_c0_seq1'),
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
  $sql = "SELECT ER.signal, B.name, B.biomaterial_id, B.description FROM {elementresult} ER
           INNER JOIN {element} E ON E.element_id = ER.element_id
           INNER JOIN {feature} F ON F.feature_id = E.feature_id
           INNER JOIN {quantification} Q ON Q.quantification_id = ER.quantification_id
           INNER JOIN {acquisition} AQ ON AQ.acquisition_id = Q.acquisition_id
           INNER JOIN {assay} A ON A.assay_id = AQ.assay_id
           INNER JOIN {assay_biomaterial} AB ON AB.assay_id = A.assay_id
           INNER JOIN {biomaterial} B ON B.biomaterial_id = AB.biomaterial_id
         WHERE
           F.feature_id = :feature_id";
  $expressions = array();
  if(!empty($form_state['value']['heatmap_feature_id'])){
    $feature_ids = explode(',', $form_state['value']['heatmap_feature_id']);
    foreach($feature_ids as $feature_id){
      // trim white space at the beginning and the end
      $feature_id = trim($feature_id);
      $expressions[] = chado_query($sql, array(":feature_id" => $feature_id));
    }
  }


  return $expressoins;
}  
